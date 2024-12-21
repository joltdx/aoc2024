#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

typedef struct XY {
    int x;
    int y;
} XY;

typedef struct Node {
    XY pos;
    int dist;
} Node;

int WIDTH = 0;
int HEIGHT = 0;
char **MAP = {0};  
int min_cheat_part_1 = 0;
int min_cheat_part_2 = 0;

const int dy[] = {-1, 1, 0, 0}; // u, d, l, r
const int dx[] = {0, 0, -1, 1}; // u, d, l, r

void read_map(const char *filename) {

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    char line[1024];
    // get map size
    // get size...
    WIDTH = 0;
    HEIGHT = 0;
    while(fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") == 0)
            break;
        if (WIDTH == 0) WIDTH = strlen(line) - 1;
        HEIGHT += 1;
    };

    rewind(file);

    // allocate memory
    MAP = (char**)malloc((HEIGHT+1)*sizeof(char*));
    for (int i = 0; i < HEIGHT; i++) {
        (MAP)[i] = (char*)malloc((WIDTH + 1) * sizeof(char));
    }

    // now get the map
    int i = 0;
    while(fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") == 0)
            break;
        strncpy(MAP[i], line, WIDTH);
        i++;
    }

    fclose(file);
}

void free_map() {
    for (int i = 0; i < HEIGHT; i++) {
        free(MAP[i]);
    }
    free(MAP);
}

typedef struct {
    int pos;
    int time;
} Step;

long part_1() {

    // just one path
    // count steps
    int path_length = 0;
    int start_x = 0;
    int start_y = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (MAP[y][x] != '#')
                path_length++;
            if (MAP[y][x] == 'S') {
                start_x = x;
                start_y = y;
            }
        }
    }

    // allocate memory
    int **path = (int**)malloc((HEIGHT+1)*sizeof(int*));
    for (int i = 0; i < HEIGHT; i++) {
        path[i] = (int*)malloc((WIDTH + 1) * sizeof(int));
        for (int j = 0; j < WIDTH; j++)
            path[i][j] = INT_MAX;
    }

    Step steps[path_length];
    int x = start_x;
    int y = start_y;
    for (int s = 0; s < path_length; s++) {
        path[y][x] = s;
        for (int i = 0; i < 4; i++) {
            if (MAP[y+dy[i]][x+dx[i]] != '#' && path[y+dy[i]][x+dx[i]] > s) {
                x = x + dx[i];
                y = y + dy[i];
                break;
            }
        }
    }

    long cheat_count = 0;
    // horizontal cheats
    for (int y = 1; y < HEIGHT; y++) {
        for (int x = 1; x < WIDTH - 2; x++) {
            if (MAP[y][x] != '#' && MAP[y][x+1] == '#' && MAP[y][x+2] != '#') {
                int cheat_value = abs(path[y][x] - path[y][x + 2]) - 2;
                if (cheat_value >= min_cheat_part_1) {
                    cheat_count++;
                }
            }
        }
    }

    // vertical cheats
    for (int x = 1; x < WIDTH; x++) {
        for (int y = 1; y < HEIGHT - 2; y++) {
            if (MAP[y][x] != '#' && MAP[y+1][x] == '#' && MAP[y+2][x] != '#') {
                int cheat_value = abs(path[y][x] - path[y+2][x]) - 2;
                if (cheat_value >= min_cheat_part_1) {
                    cheat_count++;
                }
            }
        }
    }

    for (int i = 0; i < HEIGHT; i++) {
        free(path[i]);
    }
    free(path);

    return cheat_count;
}


long part_2() {

    // just one path
    // count steps
    int path_length = 0;
    int start_x = 0;
    int start_y = 0;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (MAP[y][x] != '#')
                path_length++;
            if (MAP[y][x] == 'S') {
                start_x = x;
                start_y = y;
            }
        }
    }

    typedef struct {
        int x;
        int y;
        int steps;
    } Step;

    // allocate memory
    int **path = (int**)malloc((HEIGHT+1)*sizeof(int*));
    for (int i = 0; i < HEIGHT; i++) {
        path[i] = (int*)malloc((WIDTH + 1) * sizeof(int));
        for (int j = 0; j < WIDTH; j++)
            path[i][j] = INT_MAX;
    }

    Step steps[path_length];
    int x = start_x;
    int y = start_y;
    for (int s = 0; s < path_length + 1; s++) {
        path[y][x] = s;
        steps[s].x = x;
        steps[s].y = y;
        steps[s].steps = s;
        for (int i = 0; i < 4; i++) {
            if (MAP[y+dy[i]][x+dx[i]] != '#' && path[y+dy[i]][x+dx[i]] > s) {
                x = x + dx[i];
                y = y + dy[i];
                break;
            }
        }
    }

    long cheat_count = 0;
    for (int s = 0; s < path_length - min_cheat_part_2; s++) {
        for (int c = s + min_cheat_part_2; c < path_length; c++ ) {
            int cheat_steps = abs(steps[c].x - steps[s].x) + abs(steps[c].y - steps[s].y);
            if (cheat_steps <= 20 && c - s - cheat_steps >= min_cheat_part_2) {
                // good cheat, let's go
                cheat_count++;
            }
        }
    }

    for (int i = 0; i < HEIGHT; i++) {
        free(path[i]);
    }
    free(path);

    return cheat_count;
}

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Usage: %s <input> [test]\n", argv[0]);
        exit(1);
    }

    read_map(argv[1]);

    if (WIDTH > 20) {
        min_cheat_part_1 = 100;
        min_cheat_part_2 = 100;
    } else {
        min_cheat_part_2 = 50;
    }


    long result1 = part_1();
    printf("part 1: %ld\n", result1);

    long result2 = part_2();
    printf("part 2: %ld\n", result2);

    free_map();
}