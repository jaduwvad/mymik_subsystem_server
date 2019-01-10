#include "updater.h"
#include "receiver.h"

#define MAX_LINE 1024
#define PORT_NO 17914

using namespace std;

int main() {
    if(lockKnock())
        exit(1);

    if(fork() != 0)
        exit(1);
    else
        setEnv();
    
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

        //after get a message for specific shop, quit the connection and connect again
        str_echo(connfd);
        close(connfd);
        showTime();
    }
}

/**
 * @brief	Set environment files
 * @description	Make lock file to check if the server is on used
 * 		Make pid file to write the pid of the server program
 */
void setEnv(){
    int pid = getpid();
    ofstream pidFile(pidFileName);
    ofstream lockFile(lockFileName);

    pidFile<<pid;
    lockFile.close();
    pidFile.close();
}

/**
 * @brief	Check the lock file
 * @description	If the lock file is exist, return false
 * 		if the file is not exist,  return true
 */
bool lockKnock(){
    if(0 != access(lockFileName.c_str(), F_OK)) 
        return false;
    else
        return true;
}

/**
 * @brief	Get result file name.
 * @description	All result file name is formatted in [tag].csv.
 * 		So, it's possible to get file name by tag
 */
string getFilename(string tag){
    return tag + ".csv";
}

/**
 * @brief	Write log
 */
void writeLog(string s) {
    ofstream logFile;

    logFile.open("/var/log/mymik/update_log", ios::app);
    logFile<<s.c_str()<<endl;
    logFile.close();
}

/**
 * @brief	Write current time at log file.
 * @format	DD/MM/YYYY HH:mm
 */
void showTime(){
     char s[100];
     time_t temp;
     struct tm *timeptr;

     temp = time(NULL);
     timeptr = localtime(&temp);

     strftime(s, sizeof(s), "%m/%d/%Y %R", timeptr);
     writeLog(s);
}

bool checkMessage(string message){
    for(int i=0; i<16; i++) {
        if(message == shops[i])
            return true;
    }
    return false;
}

/**
 * @brief	Get message and update by message
 * @description	Read message from update server
 * 		If Message caught, execute the update process
 */
void str_echo(int sockfd) {
    ssize_t n;
    char line[MAX_LINE];
    char termMessage[21] = "connection terminate";
    Updater u;

    while(true) {
        if((n = read(sockfd, line, MAX_LINE)) == 0)
            break;
        line[n] = '\0';

        //if the message is for terminate, break and quit the function
        if(!strcmp(line, termMessage) || !checkMessage(line))
            break;
        string s(line);
        int updatedNum = u.updateArticle(getFilename(s), s);
        writeLog(to_string(updatedNum) + " -> " + s);
        memset(line, 0, n);
    }

    memset(line, 0, MAX_LINE);
}


