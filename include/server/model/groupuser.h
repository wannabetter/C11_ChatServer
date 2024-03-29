#ifndef CHATSERVER_GROUPUSER_H
#define CHATSERVER_GROUPUSER_H

#include "user.h"

// 群组用户，多了一个role角色信息，从User类直接继承，复用User的其它信息
class GroupUser : public User {
public:
    void setRole(std::string role) { this->role = role; }

    std::string getRole() { return this->role; }

private:
    std::string role;
};

#endif