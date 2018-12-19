#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <time.h>
#include "updater.h"

#define MAX_LINE 1024
#define PORT_NO 17914

using namespace std;

string getFilename(string tag){
    return tag + ".csv";
}

void writeLog(string s) {
    ofstream logFile;

    logFile.open("/var/log/mymik/update_log", ios::app);
    logFile<<s.c_str()<<endl;
    logFile.close();
}

void showTime(){
     char s[100];
     time_t temp;
     struct tm *timeptr;

     temp = time(NULL);
     timeptr = localtime(&temp);

     strftime(s, sizeof(s), "%m/%d/%Y %R", timeptr);
     writeLog(s);
}

void str_echo(int sockfd) {
    ssize_t n;
    char line[MAX_LINE];
    char termMessage[21] = "connection terminate";
    Updater u;

    while(true) {
        if((n = read(sockfd, line, MAX_LINE)) == 0)
            break;
        line[n] = '\0';
        if(!strcmp(line, termMessage))
            break;

        string s(line);
        int updatedNum = u.updateArticle(getFilename(s), s);
        writeLog(to_string(updatedNum) + " -> " + s);
        memset(line, 0, n);
    }

    memset(line, 0, MAX_LINE);
}

void updateShops(){
    string tags[14] = {"ERP", "KST", "VTS", "SCA", "SA", "PTR", "ERP", "RSM", "APD", "AV", "ZR", "FLC", "RKT", "KHF"};
    Updater u;
    for(string tag:tags){
        u.updateArticle(getFilename(tag), tag);
    }
}

int main() {
    //updateShops();
    if(fork() != 0)
        exit(1);

    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0) {
        writeLog("socket creation error");
        return 0;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT_NO);

    if( bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) <0) {
        writeLog("bind error");
        return 0;
    }

    listen(listenfd, 5);
    writeLog("mymik subsystem update server run...");

    while(true) {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);

        writeLog("Client connected");
        str_echo(connfd);
        close(connfd);
        showTime();
        writeLog("Client exit\n");
    }
}


