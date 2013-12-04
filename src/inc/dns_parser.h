#ifndef _DNS_PARSER_H_
#define _DNS_PARSER_H_

#include "dns.h"
#include "dns_packet_server.h"


#define REQUESTNAME "video.cs.cmu.edu"
#define HEADER_LEN    12

int parse(char* buf,size_t len,SA *address);
int parseRequestData(char *data,char *buf);
int buildResponseData(char *request,size_t len,char *ret);
#endif
