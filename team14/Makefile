#
# Name: Linghcen Li ID:lli
#
SOURCE  = src
INCLUDE = src/inc
VPATH =src
CC 		= gcc
CFLAGS= -Wall -Werror -g -std=gnu99 -I$(INCLUDE)
OBJS		= proxy.o proxy_log.o proxy_bitrate.o proxy_parser.o proxy_socket.o proxy_queue.o proxy_process.o
$(SOURCE)/%.o:%.c
	$(CC)  -c $(CFLAGS) $<

proxy: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ 

run: proxy
	./proxy log.txt 0.9 11223 1.0.0.1 0 0 3.0.0.1

run2: proxy
	./proxy log2.txt 0.9 11224 2.0.0.1 0 0 4.0.0.1
clean:
	@rm -f *~ *.o proxy
