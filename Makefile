CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -ggdb
LIBS=
FILES=src/tree.c
OUT=mytree

build:
	$(CC) $(CFLAGS) $(LIBS) -o $(OUT) $(FILES)

run:
	./$(OUT)
