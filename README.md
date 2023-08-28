# VoronoiC
Simple Voronoi diagram renderer made with [SDL2](https://www.libsdl.org/) and C

![](https://user-images.githubusercontent.com/31960595/189929905-039a72a8-69d5-4d3e-8595-907af8516c33.png)

# Implementation
There is a brute-force implementation and an optimized one which uses a [KD-Tree](https://en.wikipedia.org/wiki/K-d_tree) nearest neighbor search algorithm. The optimized version is much faster and can handle a lot more points than the brute-force one. The optimized version is the default one used in the program.

# How to use
Run the .exe and place Voronoi points on screen to see a Voronoi diagram appear

Specify the number of points to generate with a flag. For example, to generate 100 points run ```VoronoiC.exe 100```

## Controls
- Left click to place a point
- G to generate 10 random points
- P to toggle point visibility

# How to compile
In order to compile from source [GNU Make](https://www.gnu.org/software/make/) is required. Simply run the ```make``` comman inside of terminal
