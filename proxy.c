/****************************************
 * Name: Lingchen Li ID:lli
 ****************************************/
#include "proxy.h"


int verbal = 0;

int main(int argc, char **argv){
	int listenfd,connfd;
	int addrlen=sizeof(struct sockaddr_in);
	struct sockaddr_in clientaddr;
	static pool p;	

	status_t *proxy_stat;
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
 *
 */
 void initSocketPair(socket_t *pair){
 	pair->client_fd=-1;
 	pair->server_fd=-1;
 	pair->remote_addr=NULL;
 	
 	bzero(pair->host, sizeof(char)*MAXLINE);
 	pair->host_port=-1;

 	pair->method=-1;
 	pair->version=-1;
 	pair->request_type=-1;
 	bzero(pair->path, sizeof(char)*MAXLINE);

 	pair->urn=NULL;
 	pair->request_rate=-1;
 	pair->seg_num=-1;
 	pair->frag_num=-1;

 	pair->close=-1;
 	pair->contentlen=-1;

 	pair->buf_server=NULL;
	pair->buf_client=NULL;
	pair->buf_send_client=NULL;
	pair->buf_send_server=NULL;
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
