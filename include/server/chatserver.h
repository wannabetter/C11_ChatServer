#ifndef CHATSERVER_CHATSERVER_H
#define CHATSERVER_CHATSERVER_H

#include <string>

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

class ChatServer {
private:
    muduo::net::TcpServer _server;
    muduo::net::EventLoop *_loop;
private:
    void onConnection(const muduo::net::TcpConnectionPtr &conn);

    void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp timestamp);

public:
    ChatServer(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &listenAddr,
               const std::string &nameArg);

    void start();
};


#endif //CHATSERVER_CHATSERVER_H
