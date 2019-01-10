#include "updater.h"
#include <fstream>
#include <cstdlib>
#include <time.h>

using namespace std;

Updater::Updater():
    _dataDir("/var/www/vhosts/my-mik.de/Sourcing_File/") { }

Updater::~Updater() { }

/**
 * @brief	Main update function
 * @description	Inven update process:
 * 		1. Set all laststock to 1
 * 		2. Set the article's laststock to 0
 * 		   -> The article's variantID is in file, that article is active
 * 		3. Set all active to 0
 * 		4. Set the articles that it's laststock is 1 active to 0
 */
int Updater::updateArticle(string filename, string tag) {
    string updateFile = _dataDir + filename;
    string article;
    ifstream ifile;
    int i = 0;

    //Get update data from this file
    //Format : variantID, price
    ifile.open(updateFile.c_str());

    //Set all articles' laststock to 1 before update inven
    _conn.invenUpdateSetting(tag);

    while(getline(ifile, article)) {
        vector<string> articleAttr;
        myExplode(article, articleAttr);

        if(tag == "AMZ")
            if(_conn.amazonWhiteListCheck(articleAttr.at(2)) == 0)
                continue;

        string variantID = articleAttr.at(0);

        if(!_conn.articleCheck(variantID))
            continue;

        string price;
        string priceWithoutTax;

        //If the shop is wave, calc price and priceWithoutTax in wave's calc process
        if(tag == "WA"){
            float priceWithoutTaxF = atof(articleAttr.at(1).c_str()) * 1.10;
            float priceF = priceWithoutTaxF * 1.19;

            char priceC[16];
            char priceWithoutTaxC[16];

            sprintf(priceC, "%.2f", priceF);
            sprintf(priceWithoutTaxC, "%.2f", priceWithoutTaxF);

            price = priceC;
            priceWithoutTax = priceWithoutTaxC;
        }
        //If not, other shop's calc process is same (at getPriceWithoutTax)
        else {
            price = articleAttr.at(1);
            priceWithoutTax = getPriceWithoutTax(price, variantID);
        }

        string dateTime = getDateTime();

        //price -> s_articles_details purchaseprice
        _conn.priceUpdateDetail(price, variantID);
        //priceWithoutTax -> s_articles_price price
        _conn.priceUpdate(priceWithoutTax, variantID);
        //date -> s_articles_attributes attr2
        _conn.priceUpdateDate(dateTime, variantID);
        _conn.invenUpdate(variantID);

        i++;
    }

    _conn.invenActiveSetting(tag);
    _conn.invenUpdateEpilog(tag);

    return i;
}

/**
 * @brief	Get price calc by tax rate
 * @description	Get the article's taxrate
 * 		and calculate the price without tax by the taxrate
 */
string Updater::getPriceWithoutTax(string price, string variantID) {
    float priceWithoutTax;
    string taxRate = _conn.getTaxRate(variantID);
    char result[16];

    switch(atoi(taxRate.c_str())){
        case 1:
            priceWithoutTax = atof(price.c_str())/1.19;
            break;
        case 4:
            priceWithoutTax = atof(price.c_str())/1.07;
            break;
        case 5:
            priceWithoutTax = atof(price.c_str());
            break;
        default:
            priceWithoutTax = 0;
    }

    sprintf(result, "%.2f", priceWithoutTax);

    return result;
}

/**
 * @brief	Separate string by delimiter ','
 * @description	The result files separate data by ','
 * 		So separate the string from result file by delimiter
 * 		and save it to vector value to return
 */
void Updater::myExplode(string s, vector<string>& result) {
    string buff("");

    for(char n:s) {
        if(n != ',')
            buff += n;
        else if( n == ',' ) {
            result.push_back(buff);
            buff = "";
        }
    }
    if(buff != "") result.push_back(buff);
}

/**
 * @brief	Get date and time in format
 * @description	Return date and time data
 * @format	YYYY-MM-DD HH:mm:SS
 */
string Updater::getDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char temp[128];

    tstruct = *localtime(&now);
    strftime(temp, sizeof(temp), "%Y-%m-%d-%X", &tstruct);

   return temp;
}









