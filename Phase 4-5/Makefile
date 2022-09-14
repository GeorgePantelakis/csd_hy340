#NIKOLAOS NTORVAS - GEORGE-STAVROS PANTELAKIS
#AM:4013 - 4017

CC=gcc

all : Utilities.c Utilities.h create_quad_file.c create_tcode_txt_file.c create_tcode_abc_file.c tcode_Utilities.c
	flex al.l
	bison --yacc al.y
	$(CC) -o compiler analyzer.c parser.c Utilities.c create_quad_file.c create_tcode_txt_file.c create_tcode_abc_file.c tcode_Utilities.c
	$(CC) -lm -o avm AVM_executionfuncs.c AVM_helpfuncs.c AVM_mainfuncs.c AVM_stackfuncs.c 

clean:
	rm -rf analyzer.*
	rm -rf parser.*
	rm -rf compiler
	rm -rf quads.txt
	rm -rf tcode.txt
	rm -rf tcode.abc
	rm -rf avm

clear:
	rm -rf analyzer.*
	rm -rf parser.*
	rm -rf compiler
	rm -rf quads.txt
	rm -rf tcode.txt
	rm -rf tcode.abc
	rm -rf avm

conflict_test:
	bison --yacc -v al.y 
	less parser.output