#ifndef CHATSERVER_MYSQL_H
#define CHATSERVER_MYSQL_H

#include <string>
#include <mysql/mysql.h>

class MySQL {
private:
    MYSQL *_conn;
public:

    MySQL();

    ~MySQL();

    bool connect();

    bool update(std::string sql);

    MYSQL_RES *query(std::string sql);

    MYSQL *getConnection();
};


#endif //CHATSERVER_MYSQL_H
