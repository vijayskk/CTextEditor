CC=gcc
CFLAGS = -Wall -Wextra
EXEC=run
MAIN=src/main.c
TEST =tests/test.txt
main:src/main.c
	$(CC) $? $(CFLAGS) -o build/$(EXEC) 

run:
	build/$(EXEC) $(TEST)

clear:
	rm build/* 
