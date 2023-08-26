#ifndef VORONOI_H
#define VORONOI_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <limits.h>

typedef struct
{
    int x;
    int y;
    int velocity_x;
    int velocity_y;
    uint32_t color;
} point_t;

typedef struct
{
    int width;
    int height;
    int max_points;
    int point_radius;
    uint32_t point_color;
    uint32_t *color_palette;
    int palette_size;
    uint32_t *pixels;
    point_t *points;
    int point_count;
} voronoi_t;

voronoi_t voronoi_create(int width, int height, int max_points, int point_radius, uint32_t point_color, uint32_t *color_palette, int palette_size);

void voronoi_destroy(voronoi_t *v);

void fill_screen(voronoi_t *v, uint32_t color);

void voronoi_add_point(voronoi_t *v, int x, int y);

void voronoi_generate_random_points(voronoi_t *v, int count);

void voronoi_brute_force(voronoi_t *v);

void voronoi_move_points(voronoi_t *v);

void voronoi_draw(voronoi_t *v);

#endif