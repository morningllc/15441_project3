#include "proxy_socket.h"
#include "proxy_queue.h"
#include "proxy.h"

extern status_t *proxy_stat;
/*buffer manipulations*/
buffer *initBuffer(){
	buffer *b;
	if((b=(buffer *)calloc(1,sizeof(buffer)))==NULL)
		return NULL;
	b->size=BUFFERSIZE;
	b->length=0;
	b->count=0;
	if((b->buf=(char *)calloc(BUFFERSIZE,sizeof(char)))==NULL){
		free(b);
		fprintf(stderr, "error in init buf\n");
		return NULL;
	}
	b->ptr=b->buf;
	return b;
}
void freeBuffer(buffer *b){
	if(b->buf!=NULL){
		free(b->buf);
		free(b);
		b=NULL;
	}
}
int resetBuffer(buffer *b){
	freeBuffer(b);
	if((b=initBuffer())==NULL){
		return -1;
	}
	return 0;
}
int initSocketBuffer(socket_t *s){
	if((s->buf_server=initBuffer())==NULL){
		fprintf(stderr, "error in init buf_server\n");
		return -1;
	}
	if((s->buf_client=initBuffer())==NULL){
		fprintf(stderr, "error in init buf_client\n");
		return -1;
	}
	if((s->buf_send_client=initBuffer())==NULL){
		fprintf(stderr, "error in init buf_send_client\n");
		return -1;
	}
	if((s->buf_send_server=initBuffer())==NULL){
		fprintf(stderr, "error in init buf_send_server\n");
		return -1;
	}
	return 0;
}
void cleanSocketBuffer(socket_t *s){
	freeBuffer(s->buf_server);
	freeBuffer(s->buf_client);
	freeBuffer(s->buf_send_server);
	freeBuffer(s->buf_send_client);
}
int resetSocketBuffer(socket_t *s){
	cleanSocketBuffer(s);
	return initSocketBuffer(s);
}
void initSocketPair(socket_t *pair){
 	pair->client_fd=-1;
 	pair->server_fd=-1;
 	pair->remote_addr=NULL;
 	
 	bzero(pair->host, sizeof(char)*MAXLINE);
 	pair->host_port=-1;

 	pair->method=-1;
 	bzero(pair->version, sizeof(char)*MAXLINE);
 	pair->request_type=-1;
 	bzero(pair->path, sizeof(char)*MAXLINE);

 	pair->urn=NULL;
 	pair->request_rate=-1;
 	pair->seg_num=-1;
 	pair->frag_num=-1;

 	pair->close=-1;
 	pair->contentlen=-1;

 	pair->requestQueue=new_queue();
 	pair->buf_server=NULL;
	pair->buf_client=NULL;
	pair->buf_send_client=NULL;
	pair->buf_send_server=NULL;
 }
void resetSocketPair(socket_t *s){
	//fprintf(stdout, "==>in resetClient\n");
	if(s->requestQueue!=NULL){
		free_queue(s->requestQueue);
		s->requestQueue=NULL;
	}
	cleanSocketBuffer(s);
	initSocketPair(s);
	//fprintf(stdout, "==>in resetClient ret=%d\n",ret);
}
/*END*/

/*read functions*/

int clientReadByte(socket_t *pair){
	ssize_t n;
	char tmpbuf[2];
	buffer *clientbuf=pair->buf_client;

	n=recv(pair->client_fd,tmpbuf,1,MSG_DONTWAIT);
		
	if(n==0){
		return -1;
	}else if(n<0){
		if(errno==EAGAIN) 
			return 0;
		return -1;
	}else{
		if((clientbuf->length+n>MAX_REQLENGTH)){
			fprintf(stderr, "Request is too large\n");
			return -1;
			// return sendMesssage(client,"414","Request is too large");
		}else{
			memcpy(clientbuf->buf+clientbuf->length,tmpbuf,n);
			pair->buf_client->length+=n;
		}
	}	
	return -1;
}

int serverReadByte(socket_t *pair)
{
  int fd = pair->server_fd;
	char *buf = pair->buf_server->buf + pair->buf_server->length;

  if((ssize_t n=recv(fd,buf,1,NULL))<=0){
		return -1;
	}

	pair->buf_server->length+=n;

  return 0;
}

int serverReadContent(socket_t *pair)
{
  int fd = pair->server_fd;
  char *buf = pair->content_buf + pair->recv;
	int len = pair->left;

	if((ssize_t n = recv(fd, buf, len, NULL)) <= 0){
		return -1;
	}

	pair->recv+=n;
  pair->left-=n;

  return 0;
}


ssize_t readLine(buffer* src,char *usrbuf, ssize_t maxlen){
	ssize_t n;
	char *bufptr=usrbuf;
	for (n=1;n<maxlen;n++){
		if(src->count<src->length){
			
			*bufptr=*(src->ptr++);
			src->count++;
			if(*bufptr=='\n'){
				bufptr++;
				break;
			}
			bufptr++;
		}else if (n==1){
			return 0;
		}else
			break;
	}
	*bufptr=0;
	return n;
}
/*END*/
