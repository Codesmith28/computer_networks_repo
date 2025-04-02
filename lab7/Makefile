CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRCS = client.c server.c sr_arq.c
TARGETS = client server

all: $(TARGETS)

client:
	$(CC) $(CFLAGS) -o client client.c sr_arq.c -Iinclude

server:
	$(CC) $(CFLAGS) -o server server.c sr_arq.c -Iinclude

clean:
	rm -f $(TARGETS)

.PHONY: all clean
