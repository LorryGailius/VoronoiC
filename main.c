#include <voronoi.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char **argv) {

    srand(time(NULL));

    pixel_t **pixels = (pixel_t**)malloc(SCREEN_HEIGHT * sizeof(pixel_t*));

    for (size_t i = 0; i < SCREEN_HEIGHT; i++)
    {
        pixels[i] = malloc(SCREEN_WIDTH * sizeof(pixel_t));
    }
    
    if (pixels == NULL)
    {
        puts("Memory error!");
        return 0;
    }

    point_t *points = (point_t*)malloc(MAX_POINTS * sizeof(point_t));

    int size = 0;
    
    /*
    add_point(points, &size, 1, 10);
    add_point(points, &size, 50, 50);
    add_point(points, &size, 80, 60);
    add_point(points, &size, 100, 40);
    */

    generate_points(points, &size, SCREEN_WIDTH, SCREEN_HEIGHT, MAX_POINTS);

    voronoi(points, size, SCREEN_WIDTH, SCREEN_HEIGHT, pixels);

    for (size_t i = 0; i < SCREEN_HEIGHT; i++) {
        
        for (size_t j = 0; j < SCREEN_WIDTH; j++){

        }
        
    }

    for(size_t i = 0; i < SCREEN_HEIGHT; i++)
    {
        free(pixels[i]);
    }
    
    free(pixels);

    return 0; 

}