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

// /**
//  * do log
//  */
// void logWrite(char* file,char* function,char* message,...){
// 	if(logFile!=NULL){
// 		va_list ap;
// 		va_start(ap,message);

// 		time_t timetmp=time(NULL);

// 		char timeStr[80];
// 		strftime(timeStr,sizeof(timeStr),"%x %X",localtime(&timetmp));
// 		fprintf(logFile,"\ntime:%s +> ",timeStr);
// 		vfprintf(logFile,message,ap);
// 		fprintf(logFile,"\n                           in -> %s::%s\n",function,file);

// 	}else
// 		fprintf(stderr, "Error:log file is not opened\n");
// }

void logWrite(float duration, float tput,float avg-tput, int bitrate, char *client-ip,char *chunkname){
	if(logFile!=NULL){
		time_t timetmp=time(NULL);

		fprintf(logFile, "%d %.2f %.2f %.2f %d %s %s\n",(int)timetmp,duration,tput,avg-tput,bitrate,client-ip,chunkname );
	}else
		fprintf(stderr, "Error:log file is not opened\n");
}