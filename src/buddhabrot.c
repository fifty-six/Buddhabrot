//
// Yusuf Bham, 29 November 2020
//
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define MAX_X 1440
#define MAX_Y 1080

#define ITER 1000
#define ITER_MIN 200

#define POINT_ITER 1e8

#define MAX_MAG 10

double lerp(double v0, double v1, double t) {
  return (1 - t) * v0 + t * v1;
}

struct Color
{
    int r;
    int g;
    int b;
};

struct Path
{
    double re[ITER];
    double im[ITER];
};

bool mandelbrot(double re_c, double im_c, struct Path *path, int *n)
{
    double a = 0;
    double b = 0;

    int i = 0;

    for (;i < ITER; i++)
    {
        double mag = a*a + b*b;

        if (mag >= MAX_MAG)
        {
            *n = i;

            return i > ITER_MIN;
        }

        double a_new = a*a - b*b + re_c;

        b = 2*a*b + im_c;

        a = a_new;

        path -> re[i] = a;
        path -> im[i] = b;
    }

    return false;
}

void write_file(struct Color** grid, char* fname, size_t max_y, size_t max_x)
{
    FILE *fout = fopen(fname, "w");

    fprintf(fout, "P3\n");
    fprintf(fout, "%d %d\n", (int) max_x, (int) max_y);
    fprintf(fout, "255\n");

    for (size_t y = 0; y < max_y; y++ )
    {
        for (size_t x = 0; x < max_x; x++)
        {
            struct Color c = grid[y][x];

            fprintf(fout, "%d %d %d\n", c.r, c.g, c.b);
        }
    }

    fclose(fout);
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int main(void)
{
    srand(time(NULL));

    struct Color** cgrid = malloc(sizeof(struct Color*) * MAX_Y);

    for (size_t i = 0; i < MAX_Y; i++)
        cgrid[i] = malloc(sizeof(struct Color) * MAX_X);

    struct Path *path = malloc(sizeof(struct Path));

    int (*grid)[MAX_X] = malloc(sizeof(int) * MAX_X * MAX_Y);

    int *line = (int*) grid;

    memset(line, 0, sizeof(int) * MAX_X * MAX_Y);

    printf("Starting.\n");

    const int min_dim = min(MAX_X, MAX_Y);

    for (int i = 0; i < POINT_ITER; i++)
    {
        int n;

        double re_0 = 6 * drand48() - 3;
        double im_0 = 6 * drand48() - 3;

        double cr = re_0 * re_0 + im_0 * im_0;
        double ci = sqrt(cr - re_0/2 + 0.0625);

        if ((16 * cr * ci) < (5 * ci - 4 * re_0 + 1))
            continue;

        if (!mandelbrot(re_0, im_0, path, &n))
            continue;

        for (int j = 0; j < n; j++) {
            int ix = 0.3 * MAX_X * path -> re[j] + MAX_X / 2;
            int iy = 0.3 * MAX_X * path -> im[j] + MAX_Y / 2;

            if (iy >= MAX_Y || ix >= MAX_X || ix < 0 || iy < 0)
                continue;

            grid[iy][ix]++;
        }
    }

    int maximum = INT_MIN;

    for (size_t i = 0; i < MAX_X * MAX_Y; i++)
    {
        maximum = max(maximum, line[i]);
    }

    for (size_t y = 0; y < MAX_Y; y++)
    {
        for (size_t x = 0; x < MAX_X; x++)
        {
            double value = pow(grid[y][x] * 1.0 / maximum, 1.0 / 3);

            cgrid[y][x] = (struct Color) {
                .r = value * 255,
                    .g = value * 255,
                    .b = value * 255
            };
        }
    }

    printf("Finished frame.\n");

    write_file(cgrid, "out.ppm", MAX_Y, MAX_X);

    printf("Wrote file.\n");

    for (size_t i = 0; i < MAX_Y; i++)
        free(cgrid[i]);

    free(cgrid);
    free(path);

    free(grid);

    return 0;
}
//
// end of file
//
