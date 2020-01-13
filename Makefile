all: hiredis main.o stream.o consumer.o producer.o
	cc -o redis-stream main.o consumer.o producer.o stream.o ./deps/hiredis/libhiredis.a

hiredis:
	@$(MAKE) -C ./deps/hiredis

stream.o: stream.c
	cc -c stream.c

consumer.o: consumer.c
	cc -c consumer.c

producer.o: producer.c
	cc -c producer.c

main.o: main.c
	cc -c main.c

clean:
	rm -f redis-stream *.o
	@$(MAKE) -C ./deps/hiredis clean
