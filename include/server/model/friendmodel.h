#ifndef CHATSERVER_FRIENDMODEL_H
#define CHATSERVER_FRIENDMODEL_H

#include <vector>
#include "user.h"

class FriendModel {
private:

public:
    void insert(int userid, int friendid);

    std::vector<User> query(int userid);
};

#endif //CHATSERVER_FRIENDMODEL_H
