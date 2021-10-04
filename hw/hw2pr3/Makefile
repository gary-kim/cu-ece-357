badfind: badfind.c
	gcc -std=gnu11 -o badfind badfind.c

badfindleakfind: badfind.c
	gcc -std=gnu11 -fsanitize=address -fno-omit-frame-pointer -ggdb -g -Og -lrt -o badfindleakfind badfind.c
