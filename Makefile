CC = gcc

build: build_writer_reader_tatas build_producer_consumer_tatas build_philosophers_tatas

build_posix:
	gcc -o writer_reader writer_reader.c -lpthread -lrt -DPOSIX  $(CFLAGS)
	gcc -o philosophers philosophers.c -lpthread -lrt -DPOSIX  $(CFLAGS)
	gcc -o producer_consumer producer_consumer.c -lpthread -lrt -DPOSIX  $(CFLAGS)

build_writer_reader_tatas: writer_reader.c verrou_tatas.o mysemaphore.o
	$(CC) $(CFLAGS) -o writer_reader writer_reader.c verrou_tatas.o mysemaphore.o -lpthread -lrt

build_producer_consumer_tatas: producer_consumer.c verrou_tatas.o mysemaphore.o
	$(CC) $(CFLAGS) -o producer_consumer producer_consumer.c verrou_tatas.o mysemaphore.o -lpthread -lrt

build_philosophers_tatas: philosophers.c verrou_tatas.o
	$(CC) $(CFLAGS) -o philosophers philosophers.c verrou_tatas.o -lpthread -lrt

build_tas: test_and_set.c verrou.o
	$(CC) $(CFLAGS) -o test_and_set test_and_set.c verrou.o -lpthread -lrt
	
build_tatas: test_and_set.c verrou_tatas.o
	$(CC) $(CFLAGS) -o test_and_set test_and_set.c verrou_tatas.o -lpthread -lrt

verrou.o: verrou.c
	$(CC) $(CFLAGS) -c verrou.c -o verrou.o

verrou_tatas.o: verrou.c
	$(CC) $(CFLAGS) -c verrou.c -o verrou_tatas.o -DTATAS

mysemaphore.o: mysemaphore.c
	$(CC) $(CFLAGS) -c mysemaphore.c -o mysemaphore.o 


clean:
	rm -f test_and_set verrou.o verrou_tatas.o

