/**
 *
 *  HubClient.cc
 *  DrogonHub
 *
 */

#include <drogon/HubClient.h>
#include <drogon/HttpRequest.h>
#include <json/writer.h>
#include <json/reader.h>
#include <iostream>

using namespace drogon;

HubClient::HubClient(const std::string &url, const WebSocketClientPtr &client) : url_(url)
{
    parseUrl();
    if (client)
        wsClient_ = client;
    else
        wsClient_ = WebSocketClient::newWebSocketClient(host_);
}

void HubClient::parseUrl()
{
    // Simple parser: ws://host:port/path
    // Find first / after ://
    size_t protocolPos = url_.find("://");
    if (protocolPos == std::string::npos)
    {
        // Default to ws:// if missing? Or error?
        // Assume well formed for now
        host_ = url_;
        path_ = "/";
        return;
    }

    size_t pathPos = url_.find('/', protocolPos + 3);
    if (pathPos == std::string::npos)
    {
        host_ = url_;
        path_ = "/";
    }
    else
    {
        host_ = url_.substr(0, pathPos);
        path_ = url_.substr(pathPos);
    }
}

void HubClient::connect()
{
    wsClient_->setMessageHandler([this](std::string &&message, const WebSocketClientPtr &client, const WebSocketMessageType &type){
        this->onMessage(std::move(message), client, type);
    });

    auto req = HttpRequest::newHttpRequest();
    req->setPath(path_);

    wsClient_->connectToServer(req, [this](ReqResult r, const HttpResponsePtr &, const WebSocketClientPtr &){
        bool success = (r == ReqResult::Ok);
        if (connectionCallback_)
        {
            connectionCallback_(success);
        }
    });
}

void HubClient::on(const std::string &event, EventHandler &&callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_[event] = std::move(callback);
}

void HubClient::invoke(const std::string &method, const Json::Value &args)
{
    Json::Value msg;
    msg["type"] = "invoke";
    msg["target"] = method;
    msg["arguments"] = args;
    wsClient_->getConnection()->sendJson(msg);
}

void HubClient::join(const std::string &group)
{
    Json::Value msg;
    msg["type"] = "join";
    msg["group"] = group;
    wsClient_->getConnection()->sendJson(msg);
}

void HubClient::leave(const std::string &group)
{
    Json::Value msg;
    msg["type"] = "leave";
    msg["group"] = group;
    wsClient_->getConnection()->sendJson(msg);
}

void HubClient::setConnectionCallback(std::function<void(bool)> &&callback)
{
    connectionCallback_ = std::move(callback);
}

void HubClient::onMessage(std::string &&message, const WebSocketClientPtr &, const WebSocketMessageType &type)
{
    if (type != WebSocketMessageType::Text) return;

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(message, root))
    {
        LOG_ERROR << "Invalid JSON in HubClient message";
        return;
    }

    std::string msgType = root["type"].asString();
    if (msgType == "invocation")
    {
        std::string target = root["target"].asString();
        Json::Value args = root["arguments"];

        EventHandler handler;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = handlers_.find(target);
            if (it != handlers_.end())
            {
                handler = it->second;
            }
        }

        if (handler)
        {
            handler(args);
        }
    }
}
