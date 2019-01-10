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

std::string shops[16] = { "AMZ_DE", "ATN", "FLC", "ZR", "WA", "SCA", "SA", "VTS", "PTR", "ERP", "RSM", "APD", "AV", "KST", "KHF", "RKT"};
std::string lockFileName = "/var/lock/mymik_subsystem.lock";
std::string pidFileName ="/var/run/mymik_subsystem.pid";

std::string getFilename(std::string tag);
void setEnv();
bool lockKnock();
void writeLog(std::string s);
void showTime();
void str_echo(int sockfd);

