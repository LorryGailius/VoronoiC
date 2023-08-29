all:
	gcc -Isrc/Include -Lsrc/lib -o VoronoiC main.c voronoi.c -l mingw32 -lSDL2main -lSDL2 -mwindows