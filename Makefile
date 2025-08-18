compile:
	gcc -o bin/nymbolator main.c
debug:
	gcc -o bin/nymbolator main.c
	bin/nymbolator top.vhdl
execute:
	bin/nymbolator top.vhdl
test:
	gcc -o bin/test test.c && bin/test
