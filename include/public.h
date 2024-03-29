//
// Created by DELL on 2024/3/21.
//

#ifndef CHATSERVER_PUBLIC_H
#define CHATSERVER_PUBLIC_H

enum EnMsgType {
    LOGIN_MSG = 1,
    LOGIN_MSG_ACK,
    LOGINOUT_MSG,
    REG_MSG,
    REG_MSG_ACK,
    ONE_CHAT_MSG,
    ADD_FRIEND_MSG,
    CREATE_GROUP_MSG,
    ADD_GROUP_MSG,
    GROUP_CHAT_MSG
};

#endif //CHATSERVER_PUBLIC_H
