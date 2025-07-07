debug: nymbolator.c strtonum.c
	gcc -DDEBUG -o nymbolator nymbolator.c strtonum.c -I.

compile: nymbolator.c strtonum.c
	gcc -o nymbolator nymbolator.c strtonum.c -I.
