CC=gcc
CFLAGS = -Wall -Wextra
EXEC=run
MAIN=src/main.c

main:src/main.c
	$(CC) $? $(CFLAGS) -o build/$(EXEC) 

run:
	build/$(EXEC)

clear:
	rm build/*
