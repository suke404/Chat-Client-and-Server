CC = gcc

CFLAGS = -g
COMMON_OBJS = common.o



all: $(COMMON_OBJS) tcps tcpc

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c

tcps: tcpserver.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o tcps tcpserver.c $(COMMON_OBJS)


tcpc: tcpclient.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o tcpc tcpclient.c $(COMMON_OBJS) 

clean:
	rm -f *.o tcpc tcps *.dSYM