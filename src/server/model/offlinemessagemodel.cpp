//
// Created by DELL on 2024/1/22.
//

#include "server/db/db.h"
#include "server/model/offlinemessagemodel.h"

void offlineMsgModel::insert(int userid, std::string msg) {
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s');", userid, msg.c_str());
    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

void offlineMsgModel::remove(int userid) {
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d;", userid);
    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}

std::vector<std::string> offlineMsgModel::query(int userid) {
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d;", userid);
    MySQL mysql;
    std::vector<std::string> vec;
    if (mysql.connect()) {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr) {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}

