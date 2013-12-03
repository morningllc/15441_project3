#
# Name: Linghcen Li ID:lli
#
SOURCE  = src
INCLUDE = src/inc
VPATH   = src
CC 		= gcc
CFLAGS  = -Wall -Werror -g -std=gnu99 -I$(INCLUDE)
OBJS	= proxy.o proxy_log.o proxy_bitrate.o proxy_parser.o proxy_socket.o proxy_queue.o proxy_process.o
DNSOBJS = dns.o dns_log.o dns_parser.o dns_packet_server.o

DNSIP   = 5.0.0.1
DNSPORT = 11212

default: proxy nameserver

$(SOURCE)/%.o:%.c
	$(CC)  -c $(CFLAGS) $<

proxy: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ 

nameserver: $(DNSOBJS)
	$(CC) $(CFLAGS) $(DNSOBJS) -o $@ 

run: proxy
	./proxy log.txt 0.1 11223 1.0.0.1 $(DNSIP) $(DNSPORT) 3.0.0.1

run2: proxy
	./proxy log2.txt 0.1 11224 2.0.0.1 $(DNSIP) $(DNSPORT) 4.0.0.1

rundns1: nameserver
	./nameserver -r dnslog.txt $(DNSIP) $(DNSPORT)  topos/topo1/topo1.servers topos/topo1/topo1.lsa

clean:
	@rm -f *~ *.o proxy nameserver
