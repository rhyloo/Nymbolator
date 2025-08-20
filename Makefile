compile:
	gcc -o bin/nymbolator main.c
debug:
	gcc -o bin/nymbolator main.c
	bin/nymbolator top.vhdl
execute:
	bin/nymbolator top.vhdl
drawer:
	gcc -o bin/svg_drawer svg_drawer.c
	bin/svg_drawer
astar:
	gcc -o bin/astar astar.c
	bin/astar

