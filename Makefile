run:
	gcc -o writer_reader writer_reader.c -lpthread -lrt
	gcc -o philosophers philosophers.c -lpthread -lrt
	gcc -o producer_consumer producer_consumer.c -lpthread -lrt
