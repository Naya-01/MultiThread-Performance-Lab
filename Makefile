build:
	gcc -o writer_reader writer_reader.c -lpthread -lrt
	gcc -o philosophers philosophers.c -lpthread -lrt
	gcc -o producer_consumer producer_consumer.c -lpthread -lrt
	gcc -o test_and_set test_and_set.c -lpthread -lrt

build_mutex:
	gcc -o writer_reader writer_reader.c -lpthread -lrt -DMUTEX  $(CFLAGS)
	gcc -o philosophers philosophers.c -lpthread -lrt -DMUTEX  $(CFLAGS)
	gcc -o producer_consumer producer_consumer.c -lpthread -lrt -DMUTEX  $(CFLAGS)
