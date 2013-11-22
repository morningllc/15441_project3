/****************************************
 * Name: Lingchen Li ID:lli
 ****************************************/
#ifndef _LISO_LOG_H_
#define _LISO_LOG_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


void createLogFile(char *filename);
void closeLogFile() ;
void logWrite(char* file,char* function,char* message,...);

#endif