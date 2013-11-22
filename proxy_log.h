/****************************************
 * Name: Lingchen Li ID:lli
 ****************************************/
#ifndef _PROXY_LOG_H_
#define _PROXY_LOG_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


void createLogFile(char *filename);
void closeLogFile() ;
void logWrite(float duration, float tput,float avg_tput, int bitrate, char *client_ip,char *chunkname);

#endif