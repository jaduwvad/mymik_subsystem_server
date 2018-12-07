#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "updater.h"

#define MAX_LINE 1024
#define PORT_NO 50000

using namespace std;

string getFilename(string tag){
    return "aff_" + tag + ".csv";
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
        u.updateInven(getFilename(s), s);
        u.updatePrice(getFilename(s));
        memset(line, 0, n);
    }

    memset(line, 0, MAX_LINE);
}

int main() {
    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0) {
        cout<<"socket creation error"<<endl;
        return 0;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT_NO);

    if( bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) <0) {
        cout<<"bind error"<<endl;
        return 0;
    }

    listen(listenfd, 5);
    cout<<"mymik subsystem update server run..."<<endl;

    while(true) {
        len = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len);

        cout<<"Client connected"<<endl;
        str_echo(connfd);
        close(connfd);
        cout<<"Client exit"<<endl;
    }
}


