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
	int port=11223;

	status_t *proxy_stat;
	proxy_stat = initProxy(argc,argv);
	initPool(&p);	
	proxy_stat->p=&p;

	if((listenfd=open_listenfd(port))<0){
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
			addClient(connfd,&p,clientaddr);
		}
		checkClients(&p);
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
		//initClient(&p->clients[i]);
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

void addClient(int connfd,pool *p,struct sockaddr_in clientaddr){
	if(verbal)
		fprintf(stdout, "--->in add_client\n");
	p->nready--;
	int i;
	char *addr;
	for(i=0;i<FD_SETSIZE;i++){
		if(p->clients[i].fd<0){
			p->clients[i].fd=connfd;
			FD_SET(connfd,&p->read_set);
			
			addr=inet_ntoa(clientaddr.sin_addr);
			p->clients[i].remote_addr=(char *)malloc(sizeof(char)*strlen(addr));
			strcpy(p->clients[i].remote_addr,addr);

			if(connfd>p->maxfd)
				p->maxfd=connfd;
			if(i>p->maxi)
				p->maxi=i;
			break;
		}
	}
	if(i==FD_SETSIZE){
		close(connfd);
		fprintf(stderr, "too many clients\n");
	}

	if(verbal)
		fprintf(stdout, "add_client done\n");
}

void checkClients(pool *p){ 
	if(verbal)
		fprintf(stdout, "--->in check_client\n");
	
	client_t *client;
	
	for(int i=0;(i<=p->maxi)&&(p->nready>0);i++){
		
		client=&p->clients[i];

		if(client->fd>0){
			
			if(FD_ISSET(client->fd,&p->ready_write)){
				p->nready--;

				//todo send 
				

				FD_CLR(client->fd,&p->write_set);
			}
			
			if(FD_ISSET(client->fd,&p->ready_read)){
				if(verbal)
					fprintf(stdout, "read connfd=%d\n",client->fd );
				p->nready--;
				//todo read

			}
		}
	}
	if(verbal)
		fprintf(stdout, "check_client done\n");
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
