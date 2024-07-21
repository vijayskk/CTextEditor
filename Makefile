CC=gcc
CFLAGS = -Wall -Wextra
EXEC=run
main:src/main.c
	$(CC) $? $(CFLAGS) -o build/$(EXEC) 

run:
	build/run

clear:
	rm build/*
