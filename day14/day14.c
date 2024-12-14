#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long x;
    long y;
} XY;

typedef struct {
    XY p;
    XY v;
} Robot;

Robot **read_robots(const char *filename, int *num_robots) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    // Find numer of lines in file
    int num_lines = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n')
            *num_robots += 1;
    }
    if (ch != '\n')
        *num_robots += 1;

    // Allocate memory
    Robot **robots = malloc(*num_robots * sizeof(Robot *));
    for (int i = 0; i < *num_robots; i++) {
        robots[i] = malloc(sizeof(Robot));
    }

    // Get the machines
    rewind(file);
    int i = 0;
    while (fscanf(file, "p=%ld,%ld v=%ld,%ld\n", &robots[i]->p.x, &robots[i]->p.y, &robots[i]->v.x, &robots[i]->v.y) == 4)
        i++;

    fclose(file);

    return robots;
}

void free_robots(Robot **robots, const int num_robots) {
    for (int i = 0; i < num_robots; i++) {
        free(robots[i]);
    }
    free(robots);
}

XY where_is_robot(XY p, XY v, const int steps, const int width, const int height) {
    XY l;
    l.x = ( p.x + steps * v.x ) % width;
    if (l.x < 0) l.x += width;
    l.y = ( p.y + steps * v.y ) % height;
    if (l.y < 0) l.y += height;
    return l;
}

long part_1(Robot **robots, const int num, const int width, const int height) {

    long q[4] = {0};
    int qx = (width - 1) / 2;
    int qy = (height - 1) / 2;

    for (int i = 0; i < num; i++) {
        XY l = where_is_robot(robots[i]->p, robots[i]->v, 100, width, height);

        if (l.x < qx && l.y < qy)       q[0] += 1;
        else if (l.x > qx && l.y < qy)  q[1] += 1;
        else if (l.x < qx && l.y > qy)  q[2] += 1;
        else if (l.x > qx && l.y > qy)  q[3] += 1;
    }

    return q[0] * q[1] * q[2] * q[3];
}

int robots_aligned(Robot **robots, const int num, const int width, const int height) {
    // Let's go for 10 robots in a row, next to each other, x-wise?
    for (int y = 0; y < height; y++) {
        int in_a_row = 0;
        for (int x = 0; x < width; x++) {
            int yup = 0;
            for (int i = 0; i < num; i++) {
                if (robots[i]->p.x == x && robots[i]->p.y == y) {
                    yup = 1;
                    break;
                }
            }
            if (yup) {
                in_a_row++;
                if (in_a_row > 10)
                    return 1;
            } else {
                in_a_row = 0;
            }
        }
    }

    return 0;
}

void print_robots(Robot **robots, const int num, const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char yup = '.';
            for (int i = 0; i < num; i++) {
                if (robots[i]->p.x == x && robots[i]->p.y == y) {
                    yup = '*';
                    break;
                }
            }
            printf("%c", yup);
        }
        printf("\n"); 
    }
}

void step_robots(Robot **robots, const int num, const int width, const int height) {
    for (int i = 0; i < num; i++) {
        robots[i]->p.x = ( robots[i]->p.x + robots[i]->v.x ) % width;
        if (robots[i]->p.x < 0) robots[i]->p.x += width;
        robots[i]->p.y = ( robots[i]->p.y + robots[i]->v.y ) % height;
        if (robots[i]->p.y < 0) robots[i]->p.y += height;
    }
}

int part_2(Robot **robots, const int num, const int width, const int height) {

    // Hmm, let's brute force to find some aligned robots, and hope they form a tree :)
    // (Turns out they did, yaay \o/)
    for (int i = 0; i < 10000; i++) {
        if (robots_aligned(robots, num, width, height)) {
            print_robots(robots, num, width, height);
            printf("^^ %d\n\n", i);
            return i;
        }
        step_robots(robots, num, width, height);
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <input> <width> <height>\n", argv[0]);
        exit(1);
    }

    int num_robots = 0;
    Robot **robots = read_robots(argv[1], &num_robots);

    int width = atoi(argv[2]);
    int height = atoi(argv[3]);

    long result1 = part_1(robots, num_robots, width, height);
    printf("part 1: %ld\n", result1);
    long result2 = part_2(robots, num_robots, width, height);
    printf("part 2: %ld\n", result2);

    free_robots(robots, num_robots);
    return 0;
}
