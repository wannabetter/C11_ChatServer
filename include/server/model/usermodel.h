//
// Created by DELL on 2024/3/22.
//

#ifndef CHATSERVER_USERMODEL_H
#define CHATSERVER_USERMODEL_H

#include "user.h"

class UserModel {
private:

public:
    bool insert(User &user);

    User query(int id);

    bool updateState(User user);

    void resetState();
};

#endif //CHATSERVER_USERMODEL_H
