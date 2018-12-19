#include "updater.h"
#include <fstream>
#include <cstdlib>
#include <time.h>

using namespace std;

Updater::Updater():
    _dataDir("/var/www/vhosts/my-mik.de/Sourcing_File/") { }

Updater::~Updater() { }

void Updater::updateArticle(string filename, string tag) {
    string updateFile = _dataDir + filename;
    string article;
    ifstream ifile;
    int i = 0;

    ifile.open(updateFile.c_str());

    _conn.invenUpdateSetting(tag);

    while(getline(ifile, article)) {
        vector<string> articleAttr;
        myExplode(article, articleAttr);

        string variantID = articleAttr.at(0);

        if(!_conn.articleCheck(variantID))
            continue;

        string price;
        string priceWithoutTax;

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
        else {
            string price = articleAttr.at(1);
            string priceWithoutTax = getPriceWithoutTax(price, variantID, tag);
        }

        string dateTime = getDateTime();

        _conn.priceUpdateDetail(price, variantID);
        _conn.priceUpdate(priceWithoutTax, variantID);
        _conn.priceUpdateDate(dateTime, variantID);
        _conn.invenUpdate(variantID);

        i++;
    }
    cout<<filename<<" -> "<<i<<endl;
}

string Updater::getPriceWithoutTax(string price, string variantID, string tag) {
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
