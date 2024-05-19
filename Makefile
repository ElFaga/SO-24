$(CC) = gcc

build:
	$(CC) -o multithreads multithreads.c
	$(CC) -o monothread monothread.c
	
multithreads:
	$(CC) -o multithreads multithreads.c
	
monothread:
	$(CC) -o monothread monothread.c
	
clean:
	rm -rf multithreads monothread
