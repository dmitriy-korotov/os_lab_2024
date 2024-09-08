CC=gcc
CFLAGS=-I../../lab3/src -I.

all: zombi process-memory parallel-sum

zombi:
	$(CC) -o zombi zombi.c

process-memory:
	$(CC) -o process_memory process_memory.c

sum:
	$(CC) -c sum.c -o sum.o

libsum: sum
	ar rcs libsum.a sum.o

parallel-sum: libsum
	$(CC) -o parallel-sum parallel_sum.c ../../lab3/src/utils.c $(CFLAGS) -lpthread -L. -lsum

clean-all: clean-zombi clean-process-memory clean-parallel-sum

clean-sum:
	rm sum.o

clean-libsum: clean-sum
	rm libsum.a

clean-zombi:
	rm zombi

clean-process-memory:
	rm process_memory

clean-parallel-sum: clean-libsum
	rm parallel-sum
