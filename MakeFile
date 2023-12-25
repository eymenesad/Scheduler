CC = gcc
CFLAGS = -Wall

scheduler: scheduler.c
	$(CC) $(CFLAGS) -o scheduler scheduler.c

clean:
	rm -f scheduler
