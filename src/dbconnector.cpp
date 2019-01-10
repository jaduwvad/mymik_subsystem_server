#include "dbconnector.h"
#include <cstdlib>

using namespace std;

/**
 * @brief	Set database connection data.
 * @description	Insert database connection data and login id, pw at global private value.
 * 		Make database connection at global private value conn.
 */
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

/**
 * @brief	Quit mysql connection.
 * @description	Close global private value conn
 * 		to quit mysql connection.
 */
DBConnector::~DBConnector() {
    mysql_close(conn);
}

/**
 * @brief	Check if article data is in database.
 * @description	Used for all articles that need to be updated.
 * 		If the article is not in database, update doesn't work.
 */
bool DBConnector::articleCheck(string variantID) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    string query;

    // @query : match the variant number at s_articles_details id and count the matched article's number
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
/**
 * @brief	Get tax rate about matched article
 * @description	Return tax rate for the matched article.
 * 		If there is no matched article, exception raise.
 * 		But the exception handling is done at articleCheck
 */
string DBConnector::getTaxRate(string variantID){
    MYSQL_RES *res;
    MYSQL_ROW row;
    string query;
    string taxRate;

    //@query	Get matched article's taxID from s_articles 
    query = "SELECT taxID FROM s_articles WHERE id=(SELECT articleID FROM s_articles_details WHERE id=" + variantID + ")";

    res = mysqlPerformQuery(query);
    row = mysql_fetch_row(res);

    taxRate = row[0];

    mysql_free_result(res);
    
    return taxRate;
}
/**
 * @brief	Update price at s_articles_details
 */
bool DBConnector::priceUpdateDetail(string price, string variantID) {
    MYSQL_RES *res;
    string query;

    //@query	Update price at s_articles_details purchaseprice on matched row
    query = "UPDATE s_articles_details SET purchaseprice=" + price + " WHERE id=" + variantID;
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

/**
 * @brief	Update price at s_articles_prices
 */
bool DBConnector::priceUpdate(string price, string variantID) {
    MYSQL_RES *res;
    string query;

    //@query	Update price at s_articles_prices price on matched row
    query = "UPDATE s_articles_prices SET price=" + price + " WHERE articledetailsID=" + variantID;
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

/**
 * @brief	Update date at s_articles_attributes
 * @description	Insert current time and date at s_articles_attributes.
 * 		It means, this article's price is updated at this time last.
 */
bool DBConnector::priceUpdateDate(string date, string variantID) {
    MYSQL_RES *res;
    string query;

    //@query	Update date at s_articles_attributes attr2 on matched row.
    query = "UPDATE s_articles_attributes SET attr2=\"" + date + "\" WHERE articledetailsID=" + variantID;
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

/**
 * @brief	Update the shop's all article's laststock
 * @description	Set all article's laststock to 1.
 *		Articles are matched in tag.
 *		Means, articles in same shop.
 */
bool DBConnector::invenUpdateSetting(string tag) {
    MYSQL_RES *res;
    string query;

    //@query	Set all matched article's laststock to 1 
    query = "UPDATE s_articles_details SET laststock=1 WHERE ordernumber LIKE \"" + tag + "-%\"";
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

/**
 * @brief	Update matched article's laststock
 * @description	Set matched article's laststock to 0.
 * 		Means, article is being sold.
 */
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

int DBConnector::amazonWhiteListCheck(string seller){
    MYSQL_RES *res;
    MYSQL_ROW row;
    string query;
    string whiteList;

    query = "SELECT COUNT(*) FROM a_Amazon_Seller WHERE name = \"" + seller + "\"";
    res = mysqlPerformQuery(query);
    row = mysql_fetch_row(res);

    whiteList = row[0];

    mysql_free_result(res);
    
    return atoi(whiteList.c_str());
}

/**
 * @brief	Udpate article's activation.
 * @description Check articles that laststock is 1.
 * 		Than, set the articles' activation off.
 */
bool DBConnector::invenUpdateEpilog(string tag) {
    MYSQL_RES *res;
    string query;

    //@query	Set article's active to 0 which's laststock is 1 and matched at tag
    query = "UPDATE s_articles SET active=0 WHERE id IN (SELECT articleID FROM s_articles_details WHERE ordernumber LIKE \"" + tag + "-%\" AND laststock = 1)";
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

bool DBConnector::invenUpdateWrapup(string tag) {
    MYSQL_RES *res;
    string query;

    query = "UPDATE s_articles_attributes SET attr16=1 WHERE articledetailsID IN (SELECT id FROM s_articles_details WHERE ordernumber LIKE \"" + tag + "-\" AND laststock = 1)";
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

/**
 * @brief	Update the shop's all article's activation.
 * @description	Set all article's active to 1.
 *		Articles are matched in tag.
 *		Means, articles in same shop.
 */
bool DBConnector::invenActiveSetting(string tag) {
    MYSQL_RES *res;
    string query;

    query = "UPDATE s_articles SET active=1 WHERE id IN (SELECT articleID FROM s_articles_details WHERE ordernumber LIKE \"" + tag + "-%\")";
    res = mysqlPerformQuery(query);

    mysql_free_result(res);

    if(res == 0)
        return false;
    else
        return true;
}

/**
 * @brief	Execute sql query at connected database.
 * @description	Get query by parameter.
 * 		Execute query and return result.
 * 		If execute fail, return 0.
 */
MYSQL_RES* DBConnector::mysqlPerformQuery(string query) {
    if(mysql_query(conn, query.c_str())) {
        cout<<"MYSQL query error : "<<mysql_error(conn)<<endl;
        return 0;
    }
    else
        return mysql_use_result(conn);
}
