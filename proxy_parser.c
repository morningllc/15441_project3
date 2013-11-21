#include "proxy_parser.h"
#include "proxy_socket.h"
#include "proxy.h"

int parseClientRequest(socket_t *pair){
	

	size_t n;
	char buf[MAXLINE];

	if((n=readLine(pair->buf_client,buf,MAXLINE))==0){
		fprintf(stderr, "bad request - zero content\n");
		return -1;
	}

	parseMethod(pair,buf);

	return 0;
}

int parseMethod(socket_t *pair,char *buf){
	char method[MAXLINE],uri[MAXLINE],version[MAXLINE];
	char protocol[MAXLINE],host[MAXLINE],path[MAXLINE];

	if(strstr(buf,"/")==NULL){
		fprintf(stderr, "bad request - without '/' \n");
		return -1;
	}
	sscanf(buf,"%s %s %s",method,uri,version);

	/*parse method*/
	if(!strcmp(method,"GET"))
		pair->method = METHOD_GET;
	else if(!strcmp(method,"HEAD"))
		pair->method = METHOD_HEAD;
	else if(!strcmp(method,"POST"))
		pair->method = METHOD_POST;
	else{ 
		pair->method = METHOD_BAD;
	}

	/*parse version*/
	if(!strcmp(version,"HTTP/1.1"))
		pair->version = HTTP_1_1;
	if(!strcmp(version,"HTTP/1.0"))
		pair->version = HTTP_1_0;
	else{		
		pair->version = HTTP_BAD;
	}

	/*parse uri*/
	if(strstr(uri,"://")!=NULL){
		if(sscanf(uri,"%[^:]://%[^/]%s",protocol,host,path)==0){
			fprintf(stderr, "sscanf fault - uri - 1\n");
		}
	}else{
		if(sscanf(uri,"%[^/]%s",host,path)==0){
			fprintf(stderr, "sscanf fault - uri - 2\n");
		}
	}

	if(strstr(host,":")!=NULL){
		if(sscanf(host,"%[^:]:%d",pair->host,&pair->host_port)==0){
			fprintf(stderr, "sscanf fault - host\n");
		}
	}else{
		strcpy(pair->host,host);
		pair->host_port=80;
	}

	strcpy(pair->path,path);
	char *urn=strrchr(pair->path,'/');
	if(urn!=NULL) urn++;
	int bitrate,seg,frag;

	if(strstr(urn,".f4m")){
		pair->request_type = TYPE_MANIFEST;
	}else if((strchr(urn,'.')==NULL)&&(sscanf(urn,"%dSeg%d-Frag%d",&bitrate,&seg,&frag)!=0)){
		pair->urn=urn;
		pair->request_rate=bitrate;
		pair->seg_num=seg;
		pair->frag_num=frag;
		pair->request_type = TYPE_VIDEO;
	}else
		pair->request_type = TYPE_OTHER;

	return 0;
}

int parseServerResponse(socket_t *pair)
{
	char buf[MAXLINE] = {0};
	do{
		size_t n;		
		if((n=readLine(pair->buf_server,buf,MAXLINE))==0){
			fprintf(stderr, "bad request - zero content\n");
			return -1;
		}

		parseLine(pair, buf);

	}while(strncmp(buf,"\r\n",2)!=0);

	return 0;
}

int parseLine(socket_t *pair, char *buf)
{
	if(strstr(buf, "Content-Length")){
		int len = 0;
		sscanf(buf,"Content-Length: %d",&len);
		pair->contentlen = len;
	}
	return 0;
}
