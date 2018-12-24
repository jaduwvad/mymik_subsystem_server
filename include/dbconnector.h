#pragma once

#include <iostream>
#include <mysql.h>

class DBConnector {
public:
    //connect to mysql database
    DBConnector();
    //close connection
    ~DBConnector();

    //count article number matched at variantID
    bool articleCheck(std::string variantID);
    //get matched article's tax rate 
    std::string getTaxRate(std::string variantID);
    //update matched article's price at table s_articles_details
    bool priceUpdateDetail(std::string price, std::string variantID);
    //update matched article's price at table s_articles_prices
    bool priceUpdate(std::string price, std::string variantID);
    //insert update date at table s_articles_attributes, column attr2
    bool priceUpdateDate(std::string date, std::string variantID);

    //set the shop's all article's laststock to 1
    bool invenUpdateSetting(std::string tag);
    //set matched article's laststock to 0
    bool invenUpdate(std::string variantID);
    //set the shop's all article's active to 0
    bool invenActiveSetting(std::string tag);
    //inactive article that laststock is 1
    bool invenUpdateEpilog(std::string tag);

private:
    std::string server;
    std::string user;
    std::string password;
    std::string database;

    MYSQL *conn;
    MYSQL_RES *res;

    //exec query
    MYSQL_RES* mysqlPerformQuery(std::string query);
};
