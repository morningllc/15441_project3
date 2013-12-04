#ifndef _DNS_LOG_H_
#define _DNS_LOG_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


void createLogFile(char *filename);
void closeLogFile() ;
void logWrite(char* clientIP,char* query,char* response);

#endif