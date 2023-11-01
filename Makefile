CFLAGS := -Wall -g
CC:= gcc
OBJFILES = list_test.o list.o
TARGET = list_test

all: s-talk

s-talk: sock_init.o list.o thread.o
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

sock_init.o: sock_init.c
	$(CC) $(CFLAGS) -c -o $@ $<

thread.o: thread.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:	
	rm -f thread.o
	rm -f sock_init.o
	rm -f *.exe
	rm -f *.out
	rm -f s-talk

