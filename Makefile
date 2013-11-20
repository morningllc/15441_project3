#
# Name: Linghcen Li ID:lli
#
CC 		= gcc
CFLAGS= -Wall -Werror -g -std=gnu99 
OBJS		= proxy.o
.c.o:
	$(CC)  -c $(CFLAGS) $<

proxy: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)


clean:
	@rm -f *~ *.o proxy
