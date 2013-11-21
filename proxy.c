/****************************************
 * Name: Lingchen Li ID:lli
 ****************************************/
#include "proxy.h"
#include "proxy_parser.h"
#include "proxy_socket.h"
#include "proxy_process.h"

int verbal = 0;
status_t *proxy_stat;

int main(int argc, char **argv){
	int listenfd,connfd;
	int addrlen=sizeof(struct sockaddr_in);
	struct sockaddr_in clientaddr;
	static pool p;	

	
	proxy_stat = initProxy(argc,argv);
	initPool(&p);	
	proxy_stat->p=&p;

	if((listenfd=open_listenfd(proxy_stat->listenPort))<0){
		fprintf(stderr, "error in open_listenfd\n");
		return -1;
	}

	setPool(listenfd,&p);

	while(1){
		p.ready_read=p.read_set;
		p.ready_write=p.write_set;		
		
		if((p.nready=select(p.maxfd+1,&p.ready_read,&p.ready_write,NULL,NULL))<0){
			fprintf(stderr, "select error\n");
			exit(0);
		}		
			
		if(FD_ISSET(listenfd,&p.ready_read)){
			if((connfd=accept(listenfd,(SA *)&clientaddr,(socklen_t *) &addrlen))<0){
				fprintf(stderr, "http accept error\n");
				continue;
			}
			addSocketPair(connfd,&p,clientaddr);
		}
		checkSocketPairs(&p);
	}
    return 0;
}

status_t* initProxy(int argc, char **argv){
	if(argc<7||argc>8){
		fprintf(stderr,USAGE);
		exit(0);
	}
	status_t* proxy = (status_t *)malloc(sizeof(status_t));

	proxy->logFile = argv[1];
	proxy->alpha = atof(argv[2]);
	proxy->listenPort = atoi(argv[3]);
	proxy->fakeIP = argv[4];
	proxy->dnsIP = argv[5];
	proxy->dnsPort = atoi(argv[6]);
	if(argc == 8)
		proxy->wwwIP = argv[7];
	else
		proxy->wwwIP = NULL;
	
	bzero(proxy->bitrates,sizeof(int)*4);
	proxy->bitrate=0;

	return proxy;
}
/**
 *	initialize pool
 */
void initPool(pool *p){
	p->maxi=-1;
	for(int i=0;i<FD_SETSIZE;i++){
		initSocketPair(&p->pairs[i]);
	}
	p->maxfd=-1;
	FD_ZERO(&p->read_set);
	FD_ZERO(&p->write_set);
	FD_ZERO(&p->ready_read);
	FD_ZERO(&p->ready_write);	
	p->nready=0;	
}


/**
 * add two listen fds to pool
 */
void setPool(int httpfd,pool *p){
	p->maxfd=httpfd;
	FD_SET(httpfd,&p->read_set);
}

void addSocketPair(int connfd,pool *p,struct sockaddr_in clientaddr){
	if(verbal)
		fprintf(stdout, "--->in add_pair\n");
	p->nready--;
	int i;
	// char *addr;
	for(i=0;i<FD_SETSIZE;i++){
		if(p->pairs[i].client_fd<0){
			p->pairs[i].client_fd=connfd;
			FD_SET(connfd,&p->read_set);
			
			// addr=inet_ntoa(clientaddr.sin_addr);
			// p->pairs[i].remote_addr=(char *)malloc(sizeof(char)*strlen(addr));
			// strcpy(p->pairs[i].remote_addr,addr);

			initSocketBuffer(&p->pairs[i]);

			if(connfd>p->maxfd)
				p->maxfd=connfd;
			if(i>p->maxi)
				p->maxi=i;
			break;
		}
	}
	if(i==FD_SETSIZE){
		close(connfd);
		fprintf(stderr, "too many pairs\n");
	}

	if(verbal)
		fprintf(stdout, "add_pair done\n");
}

void checkSocketPairs(pool *p){ 
	if(verbal)
		fprintf(stdout, "--->in check_pair\n");
	
	socket_t *pair;
	
	for(int i=0;(i<=p->maxi)&&(p->nready>0);i++){
		
		pair=&p->pairs[i];

		if(pair->client_fd>0){
			
			if(FD_ISSET(pair->client_fd,&p->ready_write)){
				p->nready--;
				//todo send client				

				FD_CLR(pair->client_fd,&p->write_set);
			}
			
			if(FD_ISSET(pair->client_fd,&p->ready_read)){
				if(verbal)
					fprintf(stdout, "read client=%d\n",pair->client_fd);
				p->nready--;
				//todo read client
				doIt_ReadClient(pair);
			}

			if(pair->server_fd>0){
				if(FD_ISSET(pair->server_fd,&p->ready_write)){
					p->nready--;
					//todo send server				

					FD_CLR(pair->server_fd,&p->write_set);
				}
				
				if(FD_ISSET(pair->server_fd,&p->ready_read)){
					if(verbal)
						fprintf(stdout, "read server=%d\n",pair->server_fd);
					p->nready--;
					//todo read server

				}
			}
		}
	}
	if(verbal)
		fprintf(stdout, "check_pair done\n");
}

/*read data*/
void doIt_ReadClient(socket_t *pair){
	int readn;
	pool *p=proxy_stat->p;
	readn=clientReadByte(pair);

	if(readn<0){
		FD_CLR(pair->client_fd,&p->read_set);

		if(close(pair->client_fd)<0) {
			fprintf(stderr, "close fd error in doIt_ReadClient\n" );
		}
		resetSocketPair(pair);
		return;
	}

	if(strstr(pair->buf_client->buf,"\r\n\r\n")!=NULL){
		parseClientRequest(pair);
		doIt_Process(pair);
	}

}

void updateBitRate()
{
}

void doIt_ReadServer(socket_t *pair)
{
	int readn;
	if(pair->server_stat == HEADER){
		readn = serverReadByte(pair);
    if(readn < 0){
		}
		
		if(strstr(pair->buf_server->buf, "\r\n\r\n") != NULL){
			parseServerHeader(pair);
			addData(pair->buf_send_client, pair->buf_server->buf, pair->buf_server->length);
			pair->server_stat = CONTENT;
		}
	}
	else{
		readn = serverReadContent(pair);
		if(readn < 0){
		}

		if(pair->left == 0 && pair->recv == pair->contentlen){
			int type = dequeue(pair->requestQueue);
			if(type == TYPE_MANIFEST){
				parseManifestFile(pair->content_buf);
			}
			else if(type == TYPE_VIDEO){
				updateBitRate();
				addData(pair->buf_send_client, pair->content_buf, pair->contentlen);
			}
			else{
				addData(pair->buf_send_client, pair->content_buf, pair->contentlen);
			}

			pair->recv = 0;
			pair->left = 0;
			pair->contentlen = 0;
			free(pair->content_buf);
			pair->content_buf = NULL;
			pair->server_stat = HEADER;
		}
	}
}

void doIt_Process(socket_t *pair)
{
}

/**
 *	open and return a listening socket on port
 */
int open_listenfd(int port) 
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;
  
    
    if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    return -1;
  
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
           (const void *)&optval , sizeof(int)) < 0)
    return -1;

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = INADDR_ANY; 
    serveraddr.sin_port = htons((unsigned short)port); 

    if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0){
    	close(listenfd);
    	return -1;
    	}

    if (listen(listenfd, LISTENQ) < 0){
    	close(listenfd);
    	return -1;
    }
    return listenfd;
}
