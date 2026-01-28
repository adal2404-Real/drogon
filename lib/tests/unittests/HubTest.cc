#include <drogon/Hub.h>
#include <drogon/HubClient.h>
#include <drogon/drogon_test.h>
#include <iostream>

using namespace drogon;

// Mock WebSocketConnection
class MockWebSocketConnection : public WebSocketConnection
{
public:
    virtual void send(const char *msg, uint64_t len, const WebSocketMessageType type) override {}
    virtual void send(std::string_view msg, const WebSocketMessageType type) override {}

    virtual void sendJson(const Json::Value &json, const WebSocketMessageType type) override
    {
        lastJsonSent = json;
        sentCount++;
    }

    virtual const trantor::InetAddress &localAddr() const override { static trantor::InetAddress addr; return addr; }
    virtual const trantor::InetAddress &peerAddr() const override { static trantor::InetAddress addr; return addr; }
    virtual bool connected() const override { return true; }
    virtual bool disconnected() const override { return false; }
    virtual void shutdown(const CloseCode code, const std::string &reason) override {}
    virtual void forceClose() override {}
    virtual void setPingMessage(const std::string &message, const std::chrono::duration<double> &interval) override {}
    virtual void disablePing() override {}

    Json::Value lastJsonSent;
    int sentCount = 0;
};

// Mock WebSocketClient
class MockWebSocketClient : public WebSocketClient
{
public:
    virtual WebSocketConnectionPtr getConnection() override { return conn_; }

    virtual void setMessageHandler(const std::function<void(std::string &&, const WebSocketClientPtr &, const WebSocketMessageType &)> &callback) override
    {
        msgHandler_ = callback;
    }

    virtual void setConnectionClosedHandler(const std::function<void(const WebSocketClientPtr &)> &callback) override {}

    virtual void connectToServer(const HttpRequestPtr &request, const WebSocketRequestCallback &callback) override
    {
        // Simulate success
        callback(ReqResult::Ok, nullptr, nullptr);
    }

    virtual void setCertPath(const std::string &cert, const std::string &key) override {}
    virtual void addSSLConfigs(const std::vector<std::pair<std::string, std::string>> &sslConfCmds) override {}
    virtual trantor::EventLoop *getLoop() override { return nullptr; }
    virtual void stop() override {}

    void simulateMessage(std::string msg)
    {
        if (msgHandler_)
            msgHandler_(std::move(msg), nullptr, WebSocketMessageType::Text);
    }

    std::shared_ptr<MockWebSocketConnection> conn_ = std::make_shared<MockWebSocketConnection>();
    std::function<void(std::string &&, const WebSocketClientPtr &, const WebSocketMessageType &)> msgHandler_;
};

class TestHub : public Hub<TestHub>
{
public:
    void init()
    {
        registerMethod("echo", [this](const std::string &connId, const Json::Value &args){
            sendToConnection(connId, "echoBack", args);
        });

        registerMethod("broadcast", [this](const std::string &connId, const Json::Value &args){
            broadcast("notify", args);
        });

        registerMethod("groupMsg", [this](const std::string &connId, const Json::Value &args){
            sendToGroup("room1", "roomNotify", args);
        });
    }

    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/hub");
    WS_PATH_LIST_END
};

DROGON_TEST(HubTest)
{
    TestHub hub;
    hub.init();

    auto conn1 = std::make_shared<MockWebSocketConnection>();
    auto conn2 = std::make_shared<MockWebSocketConnection>();

    // Connect
    hub.handleNewConnection(nullptr, conn1);
    hub.handleNewConnection(nullptr, conn2);

    // Test 1: Echo
    {
        Json::Value msg;
        msg["type"] = "invoke";
        msg["target"] = "echo";
        Json::Value args;
        args.append("hello");
        msg["arguments"] = args;

        hub.handleNewMessage(conn1, msg.toStyledString(), WebSocketMessageType::Text);

        CHECK(conn1->sentCount == 1);
        CHECK(conn1->lastJsonSent["target"].asString() == "echoBack");
        CHECK(conn1->lastJsonSent["arguments"][0].asString() == "hello");
    }

    // Test 2: Broadcast
    {
        conn1->sentCount = 0;
        conn2->sentCount = 0;

        Json::Value msg;
        msg["type"] = "invoke";
        msg["target"] = "broadcast";
        Json::Value args;
        args.append("all");
        msg["arguments"] = args;

        hub.handleNewMessage(conn1, msg.toStyledString(), WebSocketMessageType::Text);

        CHECK(conn1->sentCount == 1);
        CHECK(conn2->sentCount == 1);
        CHECK(conn1->lastJsonSent["target"].asString() == "notify");
        CHECK(conn2->lastJsonSent["target"].asString() == "notify");
    }

    // Test 3: Groups
    {
        conn1->sentCount = 0;
        conn2->sentCount = 0;

        // Conn1 joins room1
        Json::Value joinMsg;
        joinMsg["type"] = "join";
        joinMsg["group"] = "room1";
        hub.handleNewMessage(conn1, joinMsg.toStyledString(), WebSocketMessageType::Text);

        // Conn2 sends to group
        Json::Value msg;
        msg["type"] = "invoke";
        msg["target"] = "groupMsg";
        Json::Value args;
        args.append("hi room");
        msg["arguments"] = args;

        hub.handleNewMessage(conn2, msg.toStyledString(), WebSocketMessageType::Text);

        // Conn1 should receive (in group)
        CHECK(conn1->sentCount == 1);
        CHECK(conn1->lastJsonSent["target"].asString() == "roomNotify");

        // Conn2 should NOT receive (not in group)
        CHECK(conn2->sentCount == 0);
    }

    // Cleanup
    hub.handleConnectionClosed(conn1);
    hub.handleConnectionClosed(conn2);
}

DROGON_TEST(HubClientTest)
{
    auto mockClient = std::make_shared<MockWebSocketClient>();
    HubClient client("ws://localhost", mockClient);

    bool connected = false;
    client.setConnectionCallback([&](bool s){ connected = s; });
    client.connect();
    CHECK(connected);

    // Test Invoke
    {
        Json::Value args;
        args.append("test");
        client.invoke("testMethod", args);

        CHECK(mockClient->conn_->sentCount == 1);
        CHECK(mockClient->conn_->lastJsonSent["type"].asString() == "invoke");
        CHECK(mockClient->conn_->lastJsonSent["target"].asString() == "testMethod");
    }

    // Test Receive
    {
        bool called = false;
        client.on("serverCall", [&](const Json::Value &a){
            called = true;
            CHECK(a[0].asString() == "data");
        });

        Json::Value msg;
        msg["type"] = "invocation";
        msg["target"] = "serverCall";
        Json::Value args;
        args.append("data");
        msg["arguments"] = args;

        mockClient->simulateMessage(msg.toStyledString());
        CHECK(called);
    }
}
