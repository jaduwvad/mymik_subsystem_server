#include <iostream>
#include <vector>
#include "dbconnector.h"

class Updater {
public:
    Updater();
    ~Updater();

    int updateArticle(std::string filename, std::string tag);

private:
    DBConnector _conn;

    std::string _dataDir;

    void myExplode(std::string s, std::vector<std::string>& result);
    std::string getDateTime();
    //Get matched article's price without tax. calc price by article's tax rate.
    std::string getPriceWithoutTax(std::string price, std::string variantID);
};
