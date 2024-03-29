#include <signal.h>
#include <iostream>
#include "server/chatserver.h"
#include "server/chatservice.h"

void resetHandler(int) {
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        std::cout << "command invalid! example: ./ChatServer 127.0.0.1 6000" << std::endl;
        exit(-1);
    }

    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT, resetHandler);

    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}