#ifndef CHATSERVER_GROUPUSER_H
#define CHATSERVER_GROUPUSER_H

#include "user.h"

class GroupUser : public User {
public:
    void setRole(std::string role) { this->role = role; }

    std::string getRole() { return this->role; }

private:
    std::string role;
};

#endif
