APPS = service_manager test_client test_server

all : $(APPS)

service_manager : service_manager.o binder.o
	aarch64-linux-gnu-gcc -o $@ $^

test_client : test_client.o binder.o
	aarch64-linux-gnu-gcc -o $@ $^

test_server : test_server.o binder.o
	aarch64-linux-gnu-gcc -o $@ $^

%.o : %.c
	aarch64-linux-gnu-gcc -I include -c -o $@ $<

clean:
	rm $(APPS) -f; rm -f *.o
