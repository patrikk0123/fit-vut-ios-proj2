#IOS proj2
#author: Patrik Korytar, xkoryt04
#school: FIT VUT
#date: 24.04.2021
#file desctiption: makefile

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -lpthread
SHELL = /bin/bash
MODULES = proj2.o process.o resources.o params.o error.o

all: proj2

zip:
	zip proj2.zip *.c *.h Makefile

clean:
	rm -f *.o proj2 proj2.out proj2.zip

proj2: $(MODULES)
	$(CC) $(CFLAGS) $^ -o $@
