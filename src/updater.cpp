#include "updater.h"
#include <fstream>
#include <cstdlib>
#include <time.h>

using namespace std;

Updater::Updater():
    _dataDir("/var/www/vhosts/my-mik.de/Sourcing_File/") { }

Updater::~Updater() { }

/**
 * @brief	main update function
 * @description	inven update process:
 * 		1. set all laststock to 1
 * 		2. set the article's laststock to 0
 * 		   -> the article's variantID is in file, that article is active
 * 		3. set all active to 0
 * 		4. set the articles that it's laststock is 1 active to 0
 */
int Updater::updateArticle(string filename, string tag) {
    string updateFile = _dataDir + filename;
    string article;
    ifstream ifile;
    int i = 0;

    //get update data from this file(variantID, price)
    ifile.open(updateFile.c_str());

    //set all articles' laststock to 1 before update inven
    _conn.invenUpdateSetting(tag);

    while(getline(ifile, article)) {
        vector<string> articleAttr;
        myExplode(article, articleAttr);

        string variantID = articleAttr.at(0);

        if(!_conn.articleCheck(variantID))
            continue;

        string price;
        string priceWithoutTax;

        //if the shop is wave, calc price and priceWithoutTax in wave's calc process
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
        //if not, other shop's calc process is same (at getPriceWithoutTax)
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
 * @brief	get price calc by tax rate
 * @description	get the article's taxrate
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

string Updater::getDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char temp[128];

    tstruct = *localtime(&now);
    strftime(temp, sizeof(temp), "%Y-%m-%d %X", &tstruct);

    return temp;
}
