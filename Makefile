#
# Name: Linghcen Li ID:lli
#
CC 		= gcc
CFLAGS= -Wall -Werror -g -std=gnu99 
OBJS		= proxy.o proxy_parser.o proxy_socket.o proxy_queue.o proxy_process.o
.c.o:
	$(CC)  -c $(CFLAGS) $<

proxy: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

run: proxy
	./proxy 0 0.8 11223 1.0.0.1 0 0 3.0.0.1

clean:
	@rm -f *~ *.o proxy
