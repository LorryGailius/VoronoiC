#ifndef VORONOI_H 
#define VORONOI_h

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
#define MAX_POINTS 15

typedef struct pixel_t {
    int r;
    int g;
    int b;
}pixel_t;

typedef struct point_t {
    int x;
    int y;
    pixel_t color;
}point_t;

int get_points(char *fileName, point_t points[MAX_POINTS], int *size);

int add_point(point_t points[MAX_POINTS], int *size, int x, int y);

void generate_points(point_t *points, int *size,int screen_width, int screen_height, int amount);

int voronoi(point_t *points, int size, int screen_width, int screen_height, pixel_t **pixels);

int pixel_to_ppm(pixel_t **pixels, int screen_width, int screen_height, FILE *os);

#endif