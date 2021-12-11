.PHONY: all

all: shell semaphore spinlock test_nolock

shell: shell.c shell.h locks/sem.c locks/sem.h locks/spinlock.c locks/spinlock.h 3rdparty/tas64.S 3rdparty/tas.h
	gcc -o shell -I locks -I 3rdparty -D USE_SEM --std="gnu11" shell.c shell.h locks/sem.c locks/sem.h locks/spinlock.c locks/spinlock.h 3rdparty/tas64.S 3rdparty/tas.h

semaphore: test.c test.h locks/sem.c locks/sem.h locks/spinlock.c locks/spinlock.h 3rdparty/tas64.S 3rdparty/tas.h
	gcc -o semaphore -I locks -I 3rdparty -D USE_SEM --std="gnu11" test.c test.h locks/sem.c locks/sem.h locks/spinlock.c locks/spinlock.h 3rdparty/tas64.S 3rdparty/tas.h

spinlock: test.c test.h locks/spinlock.c locks/spinlock.h 3rdparty/tas64.S 3rdparty/tas.h
	gcc -o spinlock -I locks -I 3rdparty -D USE_SPINLOCK --std="gnu11" test.c test.h locks/spinlock.c locks/spinlock.h 3rdparty/tas64.S 3rdparty/tas.h

test_nolock: test.c test.h
	gcc -o test_nolock -I locks -I 3rdparty --std="gnu11" test.c test.h

.PHONY: clean
clean:
	rm shell semaphore spinlock test_nolock

