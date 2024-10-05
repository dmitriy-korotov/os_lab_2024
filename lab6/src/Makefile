CC=gcc

--common-o:
	$(CC) -c common.c -o common.o

--libcommon: --common-o
	ar rcs libcommon.a common.o

--clean-common:
	rm -rf libcommon.a common.o &> /dev/null

client: --libcommon
	$(CC) -g client.c -L./ -lcommon -I./ -o client

clean-client: --clean-common
	rm -rf client &> /dev/null

server: --libcommon
	$(CC) -g server.c -L./ -lcommon -I./ -o server

clean-server: --clean-common
	rm -rf server &> /dev/null

clean: clean-server clean-client
