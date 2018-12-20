#pragma once

#include <iostream>
#include <mysql.h>

class DBConnector {
public:
    DBConnector();
    ~DBConnector();

    bool articleCheck(std::string variantID);
    std::string getTaxRate(std::string variantID);
    bool priceUpdateDetail(std::string price, std::string variantID);
    bool priceUpdate(std::string price, std::string variantID);
    bool priceUpdateDate(std::string date, std::string variantID);

    bool invenUpdateSetting(std::string tag);
    bool invenUpdate(std::string variantID);
    bool invenUpdateEpilog(std::string tag);

private:
    std::string server;
    std::string user;
    std::string password;
    std::string database;

    MYSQL *conn;
    MYSQL_RES *res;

    MYSQL_RES* mysqlPerformQuery(std::string query);
};
