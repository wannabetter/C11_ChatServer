#ifndef CHATSERVER_GROUPMODEL_H
#define CHATSERVER_GROUPMODEL_H

#include "group.h"
#include <string>
#include <vector>

class GroupModel {
public:

    bool createGroup(Group &group);

    void addGroup(int userid, int groupid, std::string role);

    std::vector<Group> queryGroups(int userid);

    std::vector<int> queryGroupUsers(int userid, int groupid);
};

#endif
