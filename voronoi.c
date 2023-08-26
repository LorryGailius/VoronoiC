#include "voronoi.h"

voronoi_t voronoi_create(int width, int height, int max_points, int point_radius, uint32_t point_color, uint32_t *color_palette, int palette_size)
{
    voronoi_t v;
    v.width = width;
    v.height = height;
    v.max_points = max_points;
    v.point_radius = point_radius;
    v.point_color = point_color;
    v.color_palette = color_palette;
    v.palette_size = palette_size;
    v.point_count = 0;

    v.pixels = malloc(sizeof(uint32_t) * width * height);
    if (v.pixels == NULL)
    {
        printf("Error allocating memory for pixels\n");
        exit(1);
    }

    v.points = malloc(sizeof(point_t) * max_points);
    if (v.points == NULL)
    {
        printf("Error allocating memory for points\n");
        exit(1);
    }
    return v;
}

void voronoi_destroy(voronoi_t *v)
{
    free(v->pixels);
    free(v->points);
}

void fill_screen(voronoi_t *v, uint32_t color)
{
    for (int y = 0; y < v->height; y++)
    {
        for (int x = 0; x < v->width; x++)
        {
            v->pixels[y * v->width + x] = color;
        }
    }
}

void v_draw_circle(voronoi_t *v, int cx, int cy, int radius, uint32_t color)
{
    int x0 = cx - radius;
    int y0 = cy - radius;
    int x1 = cx + radius;
    int y1 = cy + radius;

    for (int x = x0; x < x1; x++)
    {
        if (x >= 0 && x < v->width)
        {
            for (int y = y0; y < y1; y++)
            {
                if (y >= 0 && y < v->height)
                {
                    int dx = x - cx;
                    int dy = y - cy;

                    if ((dx * dx + dy * dy) <= (radius * radius))
                    {
                        v->pixels[y * v->width + x] = color;
                    }
                }
            }
        }
    }
}

void voronoi_add_point(voronoi_t *v, int x, int y)
{
    if (v->point_count > v->max_points)
    {
        return;
    }

    point_t p = {
        .x = x,
        .y = y,
        .color = v->color_palette[v->point_count % v->palette_size],
        .velocity_x = (rand() % 11 - 5),
        .velocity_y = (rand() % 11 - 5)};
    v->points[v->point_count++] = p;
}

void voronoi_generate_random_points(voronoi_t *v, int count)
{
    srand(time(NULL));
    for (int i = 0; i < count; i++)
    {
        voronoi_add_point(v, rand() % v->width, rand() % v->height);
    }
}

void v_draw_all_points(voronoi_t *v)
{
    for (int i = 0; i < v->point_count; i++)
    {
        v_draw_circle(v, v->points[i].x, v->points[i].y, v->point_radius, v->point_color);
    }
}

void voronoi_move_points(voronoi_t *v)
{
    for (int i = 0; i < v->point_count; i++)
    {
        v->points[i].x += v->points[i].velocity_x;
        v->points[i].y += v->points[i].velocity_y;

        if (v->points[i].x < 0 || v->points[i].x >= v->width)
        {
            v->points[i].velocity_x *= -1;
        }

        if (v->points[i].y < 0 || v->points[i].y >= v->height)
        {
            v->points[i].velocity_y *= -1;
        }
    }
}

int sqr_dist(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;

    return dx * dx + dy * dy;
}

void voronoi_brute_force(voronoi_t *v)
{
    for (int y = 0; y < v->height; y++)
    {
        for (int x = 0; x < v->width; x++)
        {
            int prev = 0;
            for (int i = 0; i < v->point_count; i++)
            {
                if (sqr_dist(x, y, v->points[i].x, v->points[i].y) < sqr_dist(x, y, v->points[prev].x, v->points[prev].y))
                {
                    prev = i;
                }
            }
            v->pixels[y * v->width + x] = v->points[prev].color;
        }
    }
}

void voronoi_draw(voronoi_t *v)
{
    voronoi_brute_force(v);
    v_draw_all_points(v);
}