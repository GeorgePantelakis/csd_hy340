#NIKOLAOS NTORVAS - GEORGE-STAVROS PANTELAKIS
#AM:4013 - 4017

CC=gcc

all : Utilities.c Utilities.h
	flex al.l
	bison --yacc al.y
	$(CC) -o compiler analyzer.c parser.c Utilities.c

clean:
	rm -rf analyzer.*
	rm -rf parser.*
	rm -rf compiler