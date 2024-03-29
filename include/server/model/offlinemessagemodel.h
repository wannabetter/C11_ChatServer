#ifndef CHATSERVER_OFFLINEMESSAGEMODEL_H
#define CHATSERVER_OFFLINEMESSAGEMODEL_H

#include <string>
#include <vector>

class offlineMsgModel {
private:

public:
    void insert(int userid, std::string msg);

    void remove(int userid);

    std::vector<std::string> query(int userid);

};

#endif //CHATSERVER_OFFLINEMESSAGEMODEL_H
