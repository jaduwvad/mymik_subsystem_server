#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <time.h>

#define MAX_LINE 1024
#define PORT_NO 17914

std::string getFilename(std::string tag);
void writeLog(std::string s);
void showTime();
void str_echo(int sockfd);

