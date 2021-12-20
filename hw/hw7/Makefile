.PHONY: all
all: asmcat

asmcat.o: asmcat.S
	as --64 -o asmcat.o asmcat.S

asmcat: asmcat.o
	ld -o asmcat asmcat.o

