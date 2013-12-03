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