/**
 *
 *  HubClient.h
 *  DrogonHub
 *
 */

#pragma once

#include <drogon/WebSocketClient.h>
#include <json/value.h>
#include <functional>
#include <string>
#include <map>
#include <mutex>

namespace drogon
{

class HubClient
{
public:
    /**
     * @brief Construct a new Hub Client object
     *
     * @param url The URL of the Hub, e.g. "ws://127.0.0.1:8848/hub"
     * @param client Optional WebSocketClient to use (for testing)
     */
    explicit HubClient(const std::string &url, const WebSocketClientPtr &client = nullptr);
    virtual ~HubClient() = default;

    /**
     * @brief Connect to the Hub
     *
     */
    void connect();

    using EventHandler = std::function<void(const Json::Value&)>;

    /**
     * @brief Register a handler for a specific event (method invocation from server)
     *
     * @param event The name of the event/method
     * @param callback The callback function
     */
    void on(const std::string &event, EventHandler &&callback);

    /**
     * @brief Invoke a method on the server
     *
     * @param method The name of the method
     * @param args The arguments
     */
    void invoke(const std::string &method, const Json::Value &args);

    /**
     * @brief Join a group
     *
     * @param group The group name
     */
    void join(const std::string &group);

    /**
     * @brief Leave a group
     *
     * @param group The group name
     */
    void leave(const std::string &group);

    /**
     * @brief Set the connection callback
     *
     * @param callback Function called with true on success, false on failure
     */
    void setConnectionCallback(std::function<void(bool)> &&callback);

private:
    std::string url_;
    std::string host_; // ws://ip:port
    std::string path_;
    WebSocketClientPtr wsClient_;
    std::map<std::string, EventHandler> handlers_;
    std::mutex mutex_;
    std::function<void(bool)> connectionCallback_;

    void onMessage(std::string &&message, const WebSocketClientPtr &client, const WebSocketMessageType &type);
    void parseUrl();
};

} // namespace drogon
