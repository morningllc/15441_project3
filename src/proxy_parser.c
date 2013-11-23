#include "proxy_parser.h"
#include "proxy_socket.h"
#include "proxy.h"

extern status_t *proxy_stat;
extern int verbal;
void checkBuffer(buffer *b,char *name){
	if (verbal==0) return;
	fprintf(stdout, "-------->check buf:%s<---------\n%s\n",name,b->buf);
	fprintf(stdout, "size:%d length:%d count:%d ptr:%p buf:%p\n\n",
		(int)b->size,(int)b->length,(int)b->count,
		b->ptr,b->buf);
}
int parseClientRequest(socket_t *pair){	

	size_t n;
	char buf[MAXLINE];

	checkBuffer(pair->buf_client,"buf_client");
	if((n=readLine(pair->buf_client,buf,MAXLINE))==0){
		fprintf(stderr, "bad request - zero content\n");
		return -1;
	}

	if(parseMethod(pair,buf)<0){
		fprintf(stderr, "error parse header!!!!!!!!!!!\n");
		return -1;
	}

	return 0;
}


int parseMethod(socket_t *pair,char *buf){
	char method[MAXLINE],uri[MAXLINE];
	char protocol[MAXLINE],host[MAXLINE],path[MAXLINE];

	if(strstr(buf,"/")==NULL){
		fprintf(stderr, "bad request - without '/' \n");
		return -1;
	}
	sscanf(buf,"%s %s %s",method,uri,pair->version);

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

	// /*parse version*/
	// if(!strcmp(version,"HTTP/1.1"))
	// 	pair->version = HTTP_1_1;
	// if(!strcmp(version,"HTTP/1.0"))
	// 	pair->version = HTTP_1_0;
	// else{		
	// 	pair->version = HTTP_BAD;
	// }

	/*parse uri*/
	if(strstr(uri,"://")!=NULL){
		if(sscanf(uri,"%[^:]://%[^/]%s",protocol,host,path)==0){
			fprintf(stderr, "sscanf fault - uri - 1\n");
		}
	}else{
		if(sscanf(uri,"%s",path)==0){
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
	pair->urn=strrchr(pair->path,'/');
	if(pair->urn==NULL) {
		fprintf(stderr, "urn===============null\n");
	}
	else
		pair->urn++;
	int bitrate,seg,frag;

	if(strstr(pair->urn,".f4m")){
		pair->request_type = TYPE_MANIFEST;
	}else if((strchr(pair->urn,'.')==NULL)&&(sscanf(pair->urn,"%dSeg%d-Frag%d",&bitrate,&seg,&frag)!=0)){
		// pair->urn=urn;
		pair->request_rate=bitrate;
		pair->seg_num=seg;
		pair->frag_num=frag;
		pair->request_type = TYPE_VIDEO;
	}else
		pair->request_type = TYPE_OTHER;

	return 0;
}

int parseServerHeader(socket_t *pair)
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

int parseManifestFile(char *buf)
{
  int bitrate = 0;
	char *ptr = NULL;

  if((ptr = strstr(buf, "bitrate"))==NULL)
		return -1;
  sscanf(ptr, "bitrate=\"%d\"",&bitrate);
  proxy_stat->bitrates[0] = bitrate;

  if((ptr = strstr(ptr+strlen("bitrate"), "bitrate"))==NULL)
		return -1;
  sscanf(ptr, "bitrate=\"%d\"",&bitrate);
  proxy_stat->bitrates[1] = bitrate;

  if((ptr = strstr(ptr+strlen("bitrate"), "bitrate"))==NULL)
		return -1;
  sscanf(ptr, "bitrate=\"%d\"",&bitrate);
  proxy_stat->bitrates[2] = bitrate;

  if((ptr = strstr(ptr+strlen("bitrate"), "bitrate"))==NULL)
		return -1;
  sscanf(ptr, "bitrate=\"%d\"",&bitrate);
  proxy_stat->bitrates[3] = bitrate;

	proxy_stat->bitrate = proxy_stat->bitrates[0];
	proxy_stat->t = (float)proxy_stat->bitrates[0];

	return 0;
}

int parseLine(socket_t *pair, char *buf)
{
	if(strstr(buf, "Content-Length")){
		int len = 0;
		sscanf(buf,"Content-Length: %d",&len);
		pair->recv = 0;
		pair->left = len;
		pair->contentlen = len;
		pair->content_buf = calloc(len,sizeof(char));
	}
	return 0;
}
