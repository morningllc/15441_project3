#include "dns_log.h"

static FILE *logFile;
extern int verbal;
/**
 * create logfile
 */
void createLogFile(char *filename){
	if(verbal)
		fprintf(stdout, "create log file : %s\n",filename);
	if ((logFile=fopen(filename,"w"))==NULL){
		fprintf(stderr, "Error:in creating log file\n");
		exit(0);
	}
}

/**
 * close log file
 */
void closeLogFile() {
	if(logFile!=NULL)
		if(fclose(logFile)!=0){
			fprintf(stderr, "Error:in closing log file\n");
			exit(0);	
		}
}

void logWrite(char* clientIP,char* query,char* response){
	
	if(logFile!=NULL){
		time_t timetmp=time(NULL);

		fprintf(logFile, "%d %s %s %s\n",(int)timetmp,clientIP,query,response);
		fflush(logFile);
	}else
		fprintf(stderr, "Error:log file is not opened\n");
}