#include <string>
#include <functional>

#include <json.hpp>
#include "server/chatserver.h"
#include "server/chatservice.h"


ChatServer::ChatServer(muduo::net::EventLoop *loop,
                       const muduo::net::InetAddress &listenAddr,
                       const std::string &nameArg) : _server(loop, listenAddr, nameArg), _loop(loop) {
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, std::placeholders::_1));
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2,
                                         std::placeholders::_3));
    _server.setThreadNum(4);
}

void ChatServer::start() {
    _server.start();
}

void ChatServer::onConnection(const muduo::net::TcpConnectionPtr &conn) {
    if (!conn->connected()) {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer,
                           muduo::Timestamp timestamp) {
    std::string buf = buffer->retrieveAllAsString();
    nlohmann::json js = nlohmann::json::parse(buf);
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn, js, timestamp);
}