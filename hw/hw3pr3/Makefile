.PHONY: all
all: gjshell

gjshell: gjshell.c
	gcc -ggdb -std=gnu11 -o gjshell gjshell.c

.PHONY: debug
debug:
	gdb gjshell

.PHONY: clean
clean:
	rm gjshell
