/****************************************
 * Name: Lingchen Li ID:lli
 ****************************************/
#include "proxy_log.h"
// #include "proxy.h"
static FILE *logFile;

/**
 * create logfile
 */
void createLogFile(char *filename){
	if ((logFile=fopen(filename,"w"))==NULL){
		fprintf(stderr, "Error:in creating log file\n");
		exit(0);
	}
	// logWrite("liso_log.c","createlogFile","log file created");
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


void logWrite(float duration, float tput,float avg_tput, int bitrate, char *client_ip,char *chunkname){
	if(logFile!=NULL){
		time_t timetmp=time(NULL);

		fprintf(logFile, "%d %.2f %.2f %.2f %d %s %s\n",(int)timetmp,duration,tput,avg_tput,bitrate,client_ip,chunkname );
	}else
		fprintf(stderr, "Error:log file is not opened\n");
}