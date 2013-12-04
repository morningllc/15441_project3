#include "dns.h"
#include "dns_parser.h"
#include "dns_packet_server.h"
#include "dns_log.h"
#include "dns_queue.h"

int verbal=2;
status_t* DNS_stat;

int main(int argc, char **argv){
	if(verbal)
		fprintf(stdout,"--------------proxy start-----------------\n");
	int listenfd;
	
	fd_set read_set,write_set;
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);

	signal(SIGINT,closeDNSServer);	
	DNS_stat=initDNSServer(argc,argv);	
	createLogFile(DNS_stat->logFile);

	if((listenfd=open_listenfd(DNS_stat->ip,DNS_stat->port))<0){
		fprintf(stderr, "error in open_listenfd\n");
		return -1;
	}

	while(1){
		int nready=0;	
		FD_SET(listenfd,&read_set);
		FD_SET(listenfd,&write_set);

		if(verbal>2) fprintf(stdout, "selecting......\n");
		if((nready=select(listenfd+1,&read_set,&write_set,NULL,NULL))<0){
			fprintf(stderr, "select error\n");
			exit(0);
		}		
		if(verbal>2) fprintf(stdout, "select done\n");

		if(nready>0){
			if(FD_ISSET(listenfd,&read_set)){
				doIt_Read(listenfd);
			}
			if(FD_ISSET(listenfd,&write_set)){	
				doIt_Send(listenfd);
			}
		}
	}
	return 0;
}

status_t* initDNSServer(int argc, char **argv){
	if(argc<6||argc>7){
		fprintf(stderr,USAGE);
		exit(0);
	}
	status_t* state = (status_t*)malloc(sizeof(status_t));
	if (argc==7){
		if(strcmp(argv[1],"-r")){
			fprintf(stderr,USAGE);
			exit(0);
		}
		state->logFile = argv[2];;
		state->ip = argv[3];
		state->port = atoi(argv[4]);
		state->serverFile = argv[5];
		state->LSAs = argv[6];
	}else{
		state->logFile = argv[1];;
		state->ip = argv[2];
		state->port = atoi(argv[3]);
		state->serverFile = argv[4];
		state->LSAs = argv[5];
	}
	state->send_packets=new_queue();
	return state;
}

int doIt_Read(int fd){
	if(verbal > 1) printf("dns doit read\n");
	socklen_t addrlen=sizeof(struct sockaddr_in);
	struct sockaddr_in clientaddr;
	char buf[MAXLINE];

	int recvret;
	if((recvret=recvfrom(fd, buf, BUFFERSIZE, 0, (SA *) &clientaddr, &addrlen))<0){
		fprintf(stderr, "error while recieving data\n" );
		return -1;
	}

	parse(buf,recvret,(SA *)&clientaddr);

	return 0;
}

int doIt_Send(int fd){
	send_packet_t *send = DNS_stat->sending;
	if(send != NULL){
		int sendret = sendto(fd,((char *)send->data)+(send->sent),(send->size)-(send->sent),0,&(send->address),sizeof(send->address));
		send->sent += sendret;
		if(send->sent == send->size){
			free_send_packet(send);
			DNS_stat->sending= (send_packet_t *)dequeue(DNS_stat->send_packets);
		}
	}else{
		DNS_stat->sending = (send_packet_t *)dequeue(DNS_stat->send_packets);
	}

	return 0;
}


int open_listenfd(char* ip, int port) 
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;  

    if ((listenfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0){
	    fprintf(stderr, "error in open socket\n");
	    return -1;
	}
  
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
           (const void *)&optval , sizeof(int)) < 0){
    	fprintf(stderr, "error in set socket\n");
	    return -1;
	}

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    // serveraddr.sin_addr.s_addr = INADDR_ANY; 
    inet_aton(ip,(struct in_addr *)&serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons((unsigned short)port); 
	
    if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0){
    	fprintf(stderr, "error in bind socket\n");
    	close(listenfd);
    	return -1;
    }

    // if (listen(listenfd, LISTENQ) < 0){
    // 	close(listenfd);
    // 	return -1;
    // }
    return listenfd;
}


void closeDNSServer(){
	closeLogFile();
    exit(0);
}
