#ifndef VORONOI_H
#define VORONOI_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define k 2

typedef struct Node
{
    int point[k];
    struct Node *left, *right;
    int velocity_x;
    int velocity_y;
    uint32_t color;
} node_t;

typedef struct
{
    int x;
    int y;
    int velocity_x;
    int velocity_y;
    uint32_t color;
} voronoi_point_t;

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
    voronoi_point_t *points;
    int point_count;
    node_t *root;
} voronoi_t;

voronoi_t voronoi_create(int width, int height, int max_points, int point_radius, uint32_t point_color, uint32_t *color_palette, int palette_size);

void voronoi_destroy(voronoi_t *v);

void fill_screen(voronoi_t *v, uint32_t color);

void voronoi_add_point(voronoi_t *v, int x, int y);

void voronoi_generate_random_points(voronoi_t *v, int count);

void voronoi_brute_force(voronoi_t *v);

void voronoi_move_points(voronoi_t *v);

void voronoi_kdtree(voronoi_t *v);

void voronoi_draw(voronoi_t *v, bool draw_points);
#endif