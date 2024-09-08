CC=gcc
CFLAGS=

all: zombi process-memory

zombi:
	$(CC) -o zombi zombi.c

process-memory:
	$(CC) -o process_memory process_memory.c

clean-all: clean-zombi clean-process-memory

clean-zombi:
	rm zombi

clean-process-memory:
	rm process_memory
