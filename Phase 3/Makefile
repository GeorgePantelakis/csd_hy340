#NIKOLAOS NTORVAS - GEORGE-STAVROS PANTELAKIS
#AM:4013 - 4017

CC=gcc

all : Utilities.c Utilities.h create_quad_file.c
	flex al.l
	bison --yacc al.y
	$(CC) -o compiler analyzer.c parser.c Utilities.c create_quad_file.c

clean:
	rm -rf analyzer.*
	rm -rf parser.*
	rm -rf compiler
	rm -rf quads.txt

clear:
	rm -rf analyzer.*
	rm -rf parser.*
	rm -rf compiler
	rm -rf quads.txt

conflict_test:
	bison --yacc -v al.y 
	less parser.output