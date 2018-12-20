#include "dbconnector.h"
#include <cstdlib>

using namespace std;

DBConnector::DBConnector():
    server("localhost"),
    user("mymik"),
    password("geld7914"),
    database("mmc"){
    MYSQL *connection = mysql_init(NULL);

    if(!mysql_real_connect(connection, server.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0))
        cout<<"Connection error : "<<mysql_error(connection)<<endl;
    else
        conn = connection;
}

DBConnector::~DBConnector() {
    mysql_close(conn);
}

bool DBConnector::articleCheck(string variantID) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    string query;

    query = "SELECT COUNT(*) FROM s_articles_details WHERE id = " + variantID;

    res = mysqlPerformQuery(query);
    row = mysql_fetch_row(res);

    int c = atoi(row[0]);
    mysql_free_result(res);

    if(c == 0)
        return false;
    else
        return true;
}

string DBConnector::getTaxRate(string variantID){
    MYSQL_RES *res;
    MYSQL_ROW row;
    string query;
    string taxRate;

    query = "SELECT taxID FROM s_articles WHERE id=(SELECT articleID FROM s_articles_details WHERE id=" + variantID + ")";

    res = mysqlPerformQuery(query);
    row = mysql_fetch_row(res);

    taxRate = row[0];

    mysql_free_result(res);
    
    return taxRate;
}

bool DBConnector::priceUpdateDetail(string price, string variantID) {
    MYSQL_RES *res;
    string query;

    query = "UPDATE s_articles_details SET purchaseprice=" + price + " WHERE id=" + variantID;
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

bool DBConnector::priceUpdate(string price, string variantID) {
    MYSQL_RES *res;
    string query;

    query = "UPDATE s_articles_prices SET price=" + price + " WHERE articledetailsID=" + variantID;
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

bool DBConnector::priceUpdateDate(string date, string variantID) {
    MYSQL_RES *res;
    string query;

    query = "UPDATE s_articles_attributes SET attr2=\"" + date + "\" WHERE articledetailsID=" + variantID;
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

bool DBConnector::invenUpdateSetting(string tag) {
    MYSQL_RES *res;
    string query;

    query = "UPDATE s_articles_details SET laststock=1 WHERE ordernumber LIKE \"" + tag + "-%\"";
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

bool DBConnector::invenUpdate(string variantID) {
    MYSQL_RES *res;
    string query;

    query = "UPDATE s_articles_details SET laststock=0 WHERE id = " + variantID;
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

bool DBConnector::invenUpdateEpilog(string tag) {
    MYSQL_RES *res;
    string query;

    query = "UPDATE s_articles SET active=0 WHERE id IN (SELECT articleID FROM s_articles_details WHERE ordernumber LIKE \"" + tag + "-%\" AND laststock = 1)";
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

MYSQL_RES* DBConnector::mysqlPerformQuery(string query) {
    if(mysql_query(conn, query.c_str())) {
        cout<<"MYSQL query error : "<<mysql_error(conn)<<endl;
        return 0;
    }
    else
        return mysql_use_result(conn);
}
