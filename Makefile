CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -Werror
LFLAGS=-lpthread -lrt -lm
BIN=proj2
SOURCE=proj_2.c

all:
	$(CC) $(CFLAGS) $(SOURCE) -o $(BIN) $(LFLAGS)
