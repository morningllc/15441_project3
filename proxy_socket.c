#include "proxy_socket.h"
#include "proxy.h"


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
	//logWrite(myName,"readLine","\n->usrbuf:%s",usrbuf);
	return n;
}