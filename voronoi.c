#include <voronoi.h>

int calc_dist(const int ax, const int ay, const int bx, const int by) {

    int dist = 0;

    dist = sqrt(pow((bx-ax),2) + pow(by-ay,2));

    return dist;
}

int generate_random(int lower, int upper) {

    return (rand() % (upper - lower + 1)) + lower;
}

point_t create_point(int x, int y) {

    point_t temp;

    temp.x = x;
    temp.y = y;

    temp.color.r = generate_random(0,255);
    temp.color.g = generate_random(0,255);
    temp.color.b = generate_random(0,255);

    return temp;
}

int get_points(char *fileName, point_t *points, int *size) {

    int count, tempx, tempy;

    FILE *fs = fopen(fileName, "r");

    if (fs != NULL) {

        fscanf(fs, "%d", &count);

        if (count > MAX_POINTS) { return 1; }

        for (size_t i = 0; i < count; i++) {
        
            fscanf(fs,"%d %d", &tempx, &tempy);
            points[i] = create_point(tempx, tempy);
            (*size)++;
        }
        return 0;
    }

    return -1;
}

int add_point(point_t *points, int *size, int x, int y) {
    if (*size == MAX_POINTS)
    {
        return -1;
    }
    
    points[*size] = create_point(x,y);
    (*size)++;
    return 1;
}

void generate_points(point_t *points, int *size,int screen_width, int screen_height, int amount) {

    if((amount + *size) > MAX_POINTS) {return;}

    for (size_t i = *size; i < amount; i++) {
        add_point(points,size,generate_random(0, screen_width - 1), generate_random(0, screen_height - 1));
    }
}

int voronoi(point_t *points, int size, int screen_width, int screen_height, pixel_t **pixels) {

    int dist = INT_MAX, idx = 0;

    for (size_t i = 0; i < screen_height; i++) {
        
        for (size_t j = 0; j < screen_width; j++) {
        
            for (size_t k = 0; k < size; k++)
            {
                //printf("Checking [%d %d]\n", i, j);
                int tempdist = calc_dist(j, i, points[k].x, points[k].y);
                if(tempdist < dist)
                {
                    dist = tempdist;
                    idx = k;
                }
            }
            pixels[i][j].r = points[idx].color.r;
            pixels[i][j].g = points[idx].color.g;
            pixels[i][j].b = points[idx].color.b;
            dist = INT_MAX;
        }
    }
    
    return 0;
}

int pixel_to_ppm(pixel_t **pixels, int screen_width, int screen_height, FILE *os) {

    if(pixels == NULL) {return -1;}

    fprintf(os, "P3\n");
    fprintf(os, "%d %d\n", screen_width, screen_height);
    fprintf(os, "255\n");

    for (size_t i = 0; i < screen_height; i++) {
        
        for (size_t j = 0; j < screen_width; j++){

            fprintf(os, "%d %d %d\n", pixels[screen_height - 1 - i][j].r, pixels[screen_height - 1 - i][j].g, pixels[screen_height - 1 - i][j].b);
        }
        
    }
    

    return 0;
}