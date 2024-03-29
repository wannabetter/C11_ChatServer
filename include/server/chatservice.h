//
// Created by DELL on 2024/3/21.
//

#ifndef CHATSERVER_CHATSERVICE_H
#define CHATSERVER_CHATSERVICE_H

#include <map>
#include <functional>
#include <mutex>
#include <json.hpp>
#include "server/model/usermodel.h"
#include "server/model/friendmodel.h"
#include "server/model/groupmodel.h"
#include "server/redis/redis.h"
#include "server/model/offlinemessagemodel.h"
#include <muduo/base/Logging.h>
#include <muduo/net/TcpConnection.h>

using MsgHandler = std::function<void(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js,
                                      muduo::Timestamp timestamp)>;

class ChatService {
private:
    UserModel _userModel;
    FriendModel _friendModel;
    GroupModel _groupModel;
    offlineMsgModel _offlineMsgModel;
    Redis _redis;

    std::mutex _connMutex;
    std::unordered_map<int, MsgHandler> _msgHandleMap;
    std::unordered_map<int, muduo::net::TcpConnectionPtr> _userConnMap;

private:
    ChatService();

public:
    static ChatService *instance();

    void login(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp);

    void loginout(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp);

    void reg(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp);

    void oneChat(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp);

    void addFriend(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp);

    void createGroup(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp time);

    void addGroup(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp time);

    void groupChat(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp time);

    MsgHandler getHandler(int msgid);

    void reset();

    void clientCloseException(const muduo::net::TcpConnectionPtr &conn);

    void handleRedisSubscribeMessage(int userid, std::string msg);


};

#endif //CHATSERVER_CHATSERVICE_H
