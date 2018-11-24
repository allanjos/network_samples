CC=cc
CFLAGS=-Wall -Iinclude

all: mkdirs build/server build/client

build/client: build/client.o
	$(CC) $(CFLAGS) $? -o $@

build/server: build/server.o
	$(CC) $(CFLAGS) $? -o $@

build/client.o: src/client.c
	$(CC) $(CFLAGS) -c $? -o $@

build/server.o: src/server.c
	$(CC) $(CFLAGS) -c $? -o $@

mkdirs:
	echo "mkdirs"
	if [ ! -e build ]; then mkdir build; fi

clean:
	rm -rf build/*
