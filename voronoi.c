#include "voronoi.h"

// Creates a new node for the KDTree
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

// Free memory used by the KDTree
void deleteTree(node_t *root)
{
    if (root == NULL) {
        return;
    }
    deleteTree(root->left);
    deleteTree(root->right);
    free(root);
}

// Recursive function to insert a node into the KDTree
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

// Wrapper function for insert_rec
node_t *insert(voronoi_t *v, node_t *root, int point[], uint32_t color)
{
    return insert_rec(v, root, point, 0, color);
}

// Checks if two points are the same, used for deleting nodes
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

// Compares two points in a specified dimension, used for deleting nodes
int comparePoints(int point1[], int point2[], unsigned cd)
{
    if (point1[cd] < point2[cd])
        return -1;
    if (point1[cd] > point2[cd])
        return 1;
    return 0;
}

// Finds the minimum node in a subtree, used for deleting nodes
node_t *findMin(node_t *root, unsigned cd, unsigned depth)
{
    if (root == NULL)
        return NULL;

    if (cd == depth % k)
    {
        if (root->left == NULL)
            return root;
        return findMin(root->left, cd, depth + 1);
    }

    node_t *left = findMin(root->left, cd, depth + 1);
    node_t *right = findMin(root->right, cd, depth + 1);

    node_t *min_node = root;

    if (left != NULL && comparePoints(left->point, min_node->point, cd) < 0)
        min_node = left;

    if (right != NULL && comparePoints(right->point, min_node->point, cd) < 0)
        min_node = right;

    return min_node;
}

// Deletes a specified node from the KDTree
node_t *delete_node(node_t *root, int point[], unsigned depth)
{
    if (root == NULL)
        return root;

    unsigned cd = depth % k;

    if (arePointsSame(root->point, point))
    {
        if (root->right != NULL)
        {
            node_t *min_node = findMin(root->right, cd, depth + 1);
            memcpy(root->point, min_node->point, sizeof(int) * k);
            root->color = min_node->color;
            root->right = delete_node(root->right, min_node->point, depth + 1);
        }
        else if (root->left != NULL)
        {
            node_t *min_node = findMin(root->left, cd, depth + 1);
            memcpy(root->point, min_node->point, sizeof(int) * k);
            root->color = min_node->color;
            root->right = delete_node(root->left, min_node->point, depth + 1);
            root->left = NULL;
        }
        else
        {
            free(root);
            return NULL;
        }
    }
    else
    {
        if (point[cd] < root->point[cd])
            root->left = delete_node(root->left, point, depth + 1);
        else
            root->right = delete_node(root->right, point, depth + 1);
    }

    return root;
}

// Finds the nearest neighbour of a point in the KDTree
node_t *nearest_neighbour_rec(node_t *root, int point[], unsigned depth, int *min_dist, node_t **min_node)
{
    if (root == NULL)
    {
        return NULL;
    }

    unsigned cd = depth % k;

    node_t *nearest = NULL;
    node_t *other = NULL;

    // Choose between left and right subtree
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

    // Check if splitting plane is crossed
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

// Wrapper function for nearest_neighbour_rec
node_t *nearest_neighbour(node_t *root, int point[])
{
    int min_dist = INT_MAX;
    node_t *min_node = NULL;
    return nearest_neighbour_rec(root, point, 0, &min_dist, &min_node);
}

// Initialize voronoi properties struct
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

// Free memory allocated by voronoi_create
void voronoi_destroy(voronoi_t *v)
{
    deleteTree(v->root);
    free(v->pixels);
    free(v->points);
}

// Fill screen with one color
void fill_screen(voronoi_t *v, uint32_t color)
{
    for (int i = 0, c = v->height * v->width; i < c; ++i)
    {
        v->pixels[i] = color;
    }
}

// Draw a circle on the screen
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

// Adds a point to the voronoi diagram, both to the points array and the KDTree
void voronoi_add_point(voronoi_t *v, int x, int y)
{
    if (v->point_count >= v->max_points)
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

// Used to move points around the screen space
// Removed implementation of this function from main.c becuase of poor performance
void voronoi_move_points(voronoi_t *v)
{
    for (int i = 0; i < v->point_count; i++)
    {
        // Save old coordinates
        int old_x = v->points[i].x;
        int old_y = v->points[i].y;

        // Update coordinates
        v->points[i].x += v->points[i].velocity_x;
        v->points[i].y += v->points[i].velocity_y;

        // Check if point is out of bounds
        if (v->points[i].x < 0 || v->points[i].x >= v->width)
        {
            v->points[i].velocity_x *= -1;
            v->points[i].x += v->points[i].velocity_x;
        }

        if (v->points[i].y < 0 || v->points[i].y >= v->height)
        {
            v->points[i].velocity_y *= -1;
            v->points[i].y += v->points[i].velocity_y;
        }

        // Update KDTree
        v->root = delete_node(v->root, (int[]){old_x, old_y}, 0);
        v->root = insert(v, v->root, (int[]){v->points[i].x, v->points[i].y}, v->points[i].color);
    }
}

int sqr_dist(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx * dx + dy * dy;
}

// Brute force approach to generate a voronoi diagram
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

// A more efficient approach to generate a voronoi diagram by using a KDTree data structure
void voronoi_kdtree(voronoi_t *v)
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

    voronoi_kdtree(v);

    if (draw_points)
    {
        voronoi_draw_all_points(v);
    }
}
