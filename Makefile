CC = gcc
CFLAGS = -Iinclude -Wall -g
SRC = src/astar.c src/drawer.c
# SRC = $(wildcard src/*.c)   
BIN = bin/drawer

drawer: $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)
	./$(BIN)

clean:
	rm -f $(BIN)

compile:
	gcc -o bin/nymbolator main.c
debug:
	gcc -o bin/nymbolator main.c
	bin/nymbolator top.vhdl
auto_placer:
	gcc $(CFLAGS) -o bin/auto_placer src/auto_placer.c  src/astar.c -lm
	bin/auto_placer
execute:
	bin/nymbolator top.vhdl
astar:
	gcc -o bin/astar astar.c
	bin/astar
test:
	gcc -Wall -g -o bin/test src/test.c -lm
	bin/test


