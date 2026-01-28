/**
 *
 *  Hub.h
 *  DrogonHub
 *
 */

#pragma once

#include <drogon/WebSocketController.h>
#include <drogon/utils/Utilities.h>
#include <json/value.h>
#include <json/writer.h>
#include <json/reader.h>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>

namespace drogon
{

class HubBase
{
public:
    virtual ~HubBase() {}

protected:
    using MethodHandler = std::function<void(const std::string&, const Json::Value&)>;

    void registerMethod(const std::string &name, MethodHandler &&handler)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        methods_[name] = std::move(handler);
    }

    void broadcast(const std::string &method, const Json::Value &args)
    {
        Json::Value msg;
        msg["type"] = "invocation";
        msg["target"] = method;
        msg["arguments"] = args;

        // Broadcast to all connections
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto &pair : connections_)
        {
            pair.second->sendJson(msg);
        }
    }

    void sendToConnection(const std::string &connId, const std::string &method, const Json::Value &args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = connections_.find(connId);
        if (it != connections_.end())
        {
            Json::Value msg;
            msg["type"] = "invocation";
            msg["target"] = method;
            msg["arguments"] = args;
            it->second->sendJson(msg);
        }
    }

    void sendToGroup(const std::string &groupName, const std::string &method, const Json::Value &args)
    {
        Json::Value msg;
        msg["type"] = "invocation";
        msg["target"] = method;
        msg["arguments"] = args;

        std::lock_guard<std::mutex> lock(mutex_);
        auto it = groups_.find(groupName);
        if (it != groups_.end())
        {
            for (const auto &connId : it->second)
            {
                auto connIt = connections_.find(connId);
                if (connIt != connections_.end())
                {
                    connIt->second->sendJson(msg);
                }
            }
        }
    }

    void onHubMessage(const WebSocketConnectionPtr &conn, std::string &&message, const WebSocketMessageType &type)
    {
        if (type != WebSocketMessageType::Text) return;

        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(message, root))
        {
            LOG_ERROR << "Invalid JSON in Hub message";
            return;
        }

        std::string msgType = root["type"].asString();
        std::string connId = getConnectionId(conn);

        if (msgType == "join")
        {
            std::string group = root["group"].asString();
            if (!group.empty())
            {
                std::lock_guard<std::mutex> lock(mutex_);
                groups_[group].insert(connId);
                connGroups_[connId].insert(group);
            }
        }
        else if (msgType == "leave")
        {
            std::string group = root["group"].asString();
            if (!group.empty())
            {
                std::lock_guard<std::mutex> lock(mutex_);
                groups_[group].erase(connId);
                connGroups_[connId].erase(group);
                if (groups_[group].empty())
                {
                    groups_.erase(group);
                }
            }
        }
        else if (msgType == "invoke")
        {
            std::string target = root["target"].asString();
            Json::Value args = root["arguments"];
            if (target.empty()) return;

            MethodHandler handler;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                auto it = methods_.find(target);
                if (it != methods_.end())
                {
                    handler = it->second;
                }
            }
            if (handler)
            {
                handler(connId, args);
            }
        }
    }

    void onHubConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &conn)
    {
        std::string connId = drogon::utils::getUuid();
        conn->setContext(std::make_shared<std::string>(connId));

        std::lock_guard<std::mutex> lock(mutex_);
        connections_[connId] = conn;
    }

    void onHubConnectionClosed(const WebSocketConnectionPtr &conn)
    {
        std::string connId = getConnectionId(conn);
        if (connId.empty()) return;

        std::lock_guard<std::mutex> lock(mutex_);
        connections_.erase(connId);

        auto it = connGroups_.find(connId);
        if (it != connGroups_.end())
        {
            for (const auto &group : it->second)
            {
                groups_[group].erase(connId);
                if (groups_[group].empty())
                {
                    groups_.erase(group);
                }
            }
            connGroups_.erase(it);
        }
    }

    std::string getConnectionId(const WebSocketConnectionPtr &conn)
    {
        if (conn->hasContext())
        {
            auto ctx = conn->getContext<std::string>();
            if (ctx) return *ctx;
        }
        return "";
    }

private:
    std::mutex mutex_;
    std::unordered_map<std::string, WebSocketConnectionPtr> connections_;
    std::unordered_map<std::string, std::unordered_set<std::string>> groups_;
    std::unordered_map<std::string, std::unordered_set<std::string>> connGroups_;
    std::unordered_map<std::string, MethodHandler> methods_;
};

template <typename T, bool AutoCreation = true>
class Hub : public WebSocketController<T, AutoCreation>, public HubBase
{
public:
    virtual void handleNewMessage(const WebSocketConnectionPtr &conn,
                                  std::string &&message,
                                  const WebSocketMessageType &type) override
    {
        HubBase::onHubMessage(conn, std::move(message), type);
    }

    virtual void handleNewConnection(const HttpRequestPtr &req,
                                     const WebSocketConnectionPtr &conn) override
    {
        HubBase::onHubConnection(req, conn);
    }

    virtual void handleConnectionClosed(const WebSocketConnectionPtr &conn) override
    {
        HubBase::onHubConnectionClosed(conn);
    }
};

} // namespace drogon
