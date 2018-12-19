#include <iostream>
#include <vector>
#include "dbconnector.h"

class Updater {
public:
    Updater();
    ~Updater();

    void updateArticle(std::string filename, std::string tag);
    
private:
    DBConnector _conn;

    std::string _dataDir;

    void myExplode(std::string s, std::vector<std::string>& result);
    std::string getDateTime();
    std::string getPriceWithoutTax(std::string price, std::string variantID, std::string tag);
};
