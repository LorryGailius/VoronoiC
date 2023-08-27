#include "voronoi.h"

node_t *node_create(voronoi_t *v, int arr[], uint32_t color)
{
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    if (temp == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < k; i++)
    {
        temp->point[i] = arr[i];
    }

    temp->velocity_x = rand() % 10 - 5;
    temp->velocity_y = rand() % 10 - 5;
    temp->color = color;
    temp->left = temp->right = NULL;
    return temp;
}

void deleteTree(node_t *root)
{
    if (root == NULL)
        return;
    deleteTree(root->left);
    deleteTree(root->right);
    free(root);
}

node_t *insert_rec(voronoi_t *v, node_t *root, int point[], unsigned depth, uint32_t color)
{
    if (root == NULL)
    {
        return node_create(v, point, color);
    }

    unsigned cd = depth % k;

    if (point[cd] < (root->point[cd]))
    {
        root->left = insert_rec(v, root->left, point, depth + 1, color);
    }
    else
    {
        root->right = insert_rec(v, root->right, point, depth + 1, color);
    }

    return root;
}

node_t *insert(voronoi_t *v, node_t *root, int point[], uint32_t color)
{
    return insert_rec(v, root, point, 0, color);
}

bool arePointsSame(int point1[], int point2[])
{
    for (int i = 0; i < k; i++)
    {
        if (point1[i] != point2[i])
        {
            return false;
        }
    }
    return true;
}

bool search_rec(node_t *root, int point[], unsigned depth)
{
    if (root == NULL)
    {
        return false;
    }

    if (arePointsSame(root->point, point))
    {
        return true;
    }

    unsigned cd = depth % k;

    if (point[cd] < root->point[cd])
    {
        return search_rec(root->left, point, depth + 1);
    }

    return search_rec(root->right, point, depth + 1);
}

bool search(node_t *root, int point[])
{
    return search_rec(root, point, 0);
}

node_t *nearest_neighbour_rec(node_t *root, int point[], unsigned depth, int *min_dist, node_t **min_node)
{
    if (root == NULL)
    {
        return NULL;
    }

    unsigned cd = depth % k;

    node_t *nearest = NULL;
    node_t *other = NULL;

    if (point[cd] < root->point[cd])
    {
        nearest = nearest_neighbour_rec(root->left, point, depth + 1, min_dist, min_node);
        other = root->right;
    }
    else
    {
        nearest = nearest_neighbour_rec(root->right, point, depth + 1, min_dist, min_node);
        other = root->left;
    }

    int dist_sq = 0;
    for (int i = 0; i < k; i++)
    {
        int diff = root->point[i] - point[i];
        dist_sq += diff * diff;
    }

    if (*min_dist > dist_sq)
    {
        *min_dist = dist_sq;
        *min_node = root;
    }

    int split_diff = point[cd] - root->point[cd];

    if (split_diff * split_diff < *min_dist)
    {
        node_t *temp = nearest_neighbour_rec(other, point, depth + 1, min_dist, min_node);
        if (temp != NULL)
        {
            int temp_dist_sq = 0;
            for (int i = 0; i < k; i++)
            {
                int diff = temp->point[i] - point[i];
                temp_dist_sq += diff * diff;
            }
            if (*min_dist > temp_dist_sq)
            {
                *min_node = temp;
                *min_dist = temp_dist_sq;
            }
        }
    }

    return *min_node;
}

node_t *nearest_neighbour(node_t *root, int point[])
{
    int min_dist = INT_MAX;
    node_t *min_node = NULL;
    return nearest_neighbour_rec(root, point, 0, &min_dist, &min_node);
}

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

    v.pixels = (uint32_t *)malloc(sizeof(uint32_t) * width * height);

    if (v.pixels == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    v.points = (voronoi_point_t *)malloc(sizeof(voronoi_point_t) * max_points);

    if (v.points == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }

    v.root = NULL;

    return v;
}

void voronoi_destroy(voronoi_t *v)
{
    deleteTree(v->root);
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

void draw_circle(voronoi_t *v, int cx, int cy, int radius, uint32_t color)
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

    v->points[v->point_count].x = x;
    v->points[v->point_count].y = y;
    int vely = rand() % 10 - 5;
    int velx = rand() % 10 - 5;
    v->points[v->point_count].velocity_x = velx == 0 ? rand() % 10 - 5 : velx;
    v->points[v->point_count].velocity_y = vely == 0 ? rand() % 10 - 5 : vely;
    v->points[v->point_count].color = v->color_palette[rand() % v->palette_size];

    v->root = insert(v, v->root, (int[]){x, y}, v->points[v->point_count].color);

    v->point_count++;
}

void voronoi_generate_random_points(voronoi_t *v, int count)
{
    deleteTree(v->root);
    v->root = NULL;
    v->point_count = 0;

    for (int i = 0; i < count; i++)
    {
        voronoi_add_point(v, rand() % v->width, rand() % v->height);
    }
}

void voronoi_draw_all_points(voronoi_t *v)
{
    for (int i = 0; i < v->point_count; i++)
    {
        draw_circle(v, v->points[i].x, v->points[i].y, v->point_radius, v->point_color);
    }
}

void voronoi_move_points(voronoi_t *v)
{
    // NOT IMPLEMENTED FOR PERFORMANCE REASONS
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

void voronoit_kdtree(voronoi_t *v)
{
    for (int y = 0; y < v->height; y++)
    {
        for (int x = 0; x < v->width; x++)
        {
            int point[] = {x, y};
            node_t *nearest = nearest_neighbour(v->root, point);
            v->pixels[y * v->width + x] = nearest->color;
        }
    }
}

void voronoi_draw(voronoi_t *v, bool draw_points)
{
    if (v->point_count == 0)
    {
        return;
    }

    // voronoi_brute_force(v);
    voronoit_kdtree(v);

    if (draw_points)
    {
        voronoi_draw_all_points(v);
    }
}
