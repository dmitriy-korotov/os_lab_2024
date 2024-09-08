CC=gcc
CFLAGS=-I. -lm

all: sequential_min_max parallel_min_max run_sequential_min_max test

__test_buid: find_min_max.o
	$(CC) -o test find_min_max.o test.c $(CFLAGS) -lcunit

test: __test_buid
	./test

run_sequential_min_max:
	$(CC) -o run_sequential_min_max run_sequential_min_max.c $(CFLAGS)

sequential_min_max : utils.o find_min_max.o
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o :
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.o
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max run_sequential_min_max test
