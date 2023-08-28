all:
	gcc -Isrc/Include -Lsrc/lib -o Run main.c voronoi.c -l mingw32 -lSDL2main -lSDL2