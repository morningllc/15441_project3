#include "proxy_process.h"
#include "proxy.h"
#include "proxy_socket.h"
#include "proxy_parser.h"
extern status_t *proxy_stat;

int buildRequestContent(socket_t *pair){
	 char request[BUFFERSIZE];
	 char header[BUFFERSIZE];

	 buildRequestHeader(pair, header);

	 strcpy(request,"GET");
	 strcat(request," ");
	 switch(pair->request_type){
	 	case TYPE_MANIFEST:
	 		pair->urn=0;
	 		return buildManifestContent(pair,header);
	 	case TYPE_VIDEO:
	 		pair->urn=0;
	 		sprintf(pair->path,"%s%dSeg%d-Frag%d ",pair->path,
	 			proxy_stat->bitrate,pair->seg_num,pair->frag_num);
	 		break;
	 	default:
	 		break;
	 }	

	 strcat(request,pair->path);
	 strcat(request," ");
	 strcat(request,pair->version);
	 if(strstr(request,"\r\n")==NULL)
	 	strcat(request,"\r\n");
	 strcat(request,header);


	 if(addData(pair->buf_send_server,(void *)request,strlen(request)))
	 	return -1;
	 enqueue(pair->requestQueue,pair->request_type);
	 pair->time = time(NULL);
	 return 0;
}
int buildManifestContent(socket_t *pair,char *header){
	char request[BUFFERSIZE];
	char request_nolist[BUFFERSIZE];

	sprintf(request,"GET %sbig_buck_bunny.f4m %s",pair->path,pair->version);
	if(strstr(request,"\r\n")==NULL)
	 	strcat(request,"\r\n");
	strcat(request,header);

	sprintf(request_nolist,"GET %sbig_buck_bunny_nolist.f4m %s",pair->path,pair->version);
	if(strstr(request_nolist,"\r\n")==NULL)
	 	strcat(request_nolist,"\r\n");
	strcat(request_nolist,header);

	if(addData(pair->buf_send_server,(void *)request,strlen(request))){
		fprintf(stderr, "add data error - add manifest data\n");
	 	return -1;
	 }
	 enqueue(pair->requestQueue,TYPE_MANIFEST);
	 if(addData(pair->buf_send_server,(void *)request_nolist,strlen(request))){
	 	fprintf(stderr, "add data error - add nolist manifest data\n");
	 	return -1;
	 }
	 enqueue(pair->requestQueue,TYPE_OTHER);

	 return 0;
}
int buildRequestHeader(socket_t *pair, char* header){
	char tmpbuf[BUFFERSIZE]={0};
	ssize_t n;

	while((n=readLine(pair->buf_client,tmpbuf,BUFFERSIZE))!=0){
		if(n<0) return -1;
		if(!strcmp(tmpbuf,"\r\n"))
			break;
		else if(!strncasecmp(tmpbuf,"Connection: close",strlen("Connection: close"))){
			pair->close=1;
			strcat(header,tmpbuf);
		}else
			strcat(header,tmpbuf);
	}
	strcat(header,"\r\n");
	return 0;
}

int open_serverfd(socket_t *pair)
{
  char *server_ip = proxy_stat->wwwIP;
  char server_port[] = "8080";
  char *fake_ip = proxy_stat->fakeIP;
	char rand_port[] = "5555";

	int status, sock;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  struct addrinfo *servinfo, *addr;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(server_ip, server_port, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo error\n");
		return -1;
	}

  if((status = getaddrinfo(fake_ip, rand_port, NULL, &addr)) != 0){
		fprintf(stderr, "getaddrinfo error\n");
    return -1;
	}

	if((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		fprintf(stderr, "Socket failed\n");
		return -1;
	}

	if(bind(sock, (struct sockaddr *)addr, sizeof(*addr))){
		fprintf(stderr, "Failed binding socket\n");
		return -1;
	}

  if(connect(sock, servinfo->ai_addr, servinfo->ai_addrlen)){
		fprintf(stderr, "Connect failed\n");
		return -1;
	}

	freeaddrinfo(servinfo);
	freeaddrinfo(addr);

  pair->server_fd = sock;

	return 1;
}


int addData(buffer *b,void* data,size_t len){
	//fprintf(stdout, "--->addData\n");

	int offset=b->ptr-b->buf;

	if(b->length+len>b->size){

		size_t ex=((b->length+len)>(b->size+BUFFERSIZE))
		?(b->length+len):(b->size+BUFFERSIZE);

		char *new=(char *)realloc(b->buf,ex);
		if(new==NULL){
			fprintf(stderr, "Error: in realloc.\n");
			return -1;
		}

		b->buf=new;
		b->ptr=new+offset;
		b->size=ex;
	}
	if(memcpy(b->ptr,data,len)==NULL){
		fprintf(stderr, "Error: in memcpy.\n");
		return -1;
	}
	b->length+=len;
	b->ptr+=len;
	//fprintf(stdout, "--->addData done\n");
	return 0;
}