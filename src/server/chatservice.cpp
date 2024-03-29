//
// Created by DELL on 2024/3/21.
//
#include "public.h"
#include <vector>
#include "server/chatservice.h"

ChatService *ChatService::instance() {
    static ChatService service;
    return &service;
}

ChatService::ChatService() {
    _msgHandleMap.insert({EnMsgType::LOGIN_MSG,
                          std::bind(&ChatService::login, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)});
    _msgHandleMap.insert({EnMsgType::LOGINOUT_MSG,
                          std::bind(&ChatService::loginout, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)});
    _msgHandleMap.insert({EnMsgType::REG_MSG,
                          std::bind(&ChatService::reg, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)});
    _msgHandleMap.insert({EnMsgType::ONE_CHAT_MSG,
                          std::bind(&ChatService::oneChat, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)});
    _msgHandleMap.insert({EnMsgType::ADD_FRIEND_MSG,
                          std::bind(&ChatService::addFriend, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)});
    _msgHandleMap.insert({EnMsgType::CREATE_GROUP_MSG,
                          std::bind(&ChatService::createGroup, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)});
    _msgHandleMap.insert({EnMsgType::ADD_GROUP_MSG,
                          std::bind(&ChatService::addGroup, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)});
    _msgHandleMap.insert({EnMsgType::GROUP_CHAT_MSG,
                          std::bind(&ChatService::groupChat, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3)});

    if (_redis.connect()) {
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, std::placeholders::_1,
                                             std::placeholders::_2));
    }
}

void ChatService::reset() {
    _userModel.resetState();
}

void ChatService::login(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp) {
    int id = js["id"].get<int>();
    std::string pwd = js["password"];

    User user = _userModel.query(id);
    if (user.getId() == id && user.getPwd() == pwd) {
        if (user.getState() == "online") {
            nlohmann::json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账号已经登陆!";
            conn->send(response.dump());
        } else {

            {
                std::lock_guard<std::mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            _redis.subscribe(id);

            user.setState("online");
            _userModel.updateState(user);

            nlohmann::json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            std::vector<std::string> vec = _offlineMsgModel.query(id);
            if (!vec.empty()) {
                response["offlinemsg"] = vec;
                _offlineMsgModel.remove(id);
            }

            std::vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty()) {
                std::vector<std::string> vec2;
                for (User &user: userVec) {
                    nlohmann::json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }
            conn->send(response.dump());
        }
    } else {

        nlohmann::json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或密码错误!";
        conn->send(response.dump());
    }
}

void ChatService::loginout(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp) {
    int userid = js["id"].get<int>();

    {
        std::lock_guard<std::mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end()) {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid);

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}

void ChatService::reg(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp) {
    std::string name = js["name"];
    std::string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state) {
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    } else {
        nlohmann::json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

MsgHandler ChatService::getHandler(int msgid) {
    auto it = _msgHandleMap.find(msgid);
    if (it == _msgHandleMap.end()) {
        return [=](const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp) {
            LOG_ERROR << "msgid" << msgid << " can not find handler!";
        };
    } else {
        return _msgHandleMap[msgid];
    }
}


void ChatService::clientCloseException(const muduo::net::TcpConnectionPtr &conn) {
    User user;
    {
        std::lock_guard<std::mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++) {
            if (it->second == conn) {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    _redis.unsubscribe(user.getId());

    if (user.getId() != -1) {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

void ChatService::oneChat(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp) {
    int toid = js["toid"].get<int>();

    {
        std::lock_guard<std::mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end()) {
            // toid在线，转发消息   服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return;
        }
    }

    // 查询toid是否在线
    User user = _userModel.query(toid);
    if (user.getState() == "online") {
        _redis.publish(toid, js.dump());
        return;
    }

    // toid不在线，存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

void ChatService::addFriend(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp timestamp) {
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    _friendModel.insert(userid, friendid);
}

void ChatService::createGroup(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp time) {
    int userid = js["id"].get<int>();
    std::string name = js["groupname"];
    std::string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group)) {
        // 存储群组创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

// 加入群组业务
void ChatService::addGroup(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp time) {
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const muduo::net::TcpConnectionPtr &conn, nlohmann::json &js, muduo::Timestamp time) {
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    std::vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);
    std::lock_guard<std::mutex> lock(_connMutex);
    for (int id: useridVec) {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end()) {
            // 转发群消息
            it->second->send(js.dump());
        } else {
            // 查询toid是否在线
            User user = _userModel.query(id);
            if (user.getState() == "online") {
                _redis.publish(id, js.dump());
            } else {
                // 存储离线群消息
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

void ChatService::handleRedisSubscribeMessage(int userid, std::string msg) {
    std::lock_guard<std::mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end()) {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}