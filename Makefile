CC = gcc
CFLAGS = -Iinclude -Wall -g
LDLIBS = -lm

SRC = src/auto_router.c src/stb_impl.c src/auto_placer.c src/drawer.c src/main.c
BIN = bin/nymbolator

compile: $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDLIBS)
	./$(BIN)

clean:
	rm -r -f $(BIN) bin/

