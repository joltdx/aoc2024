#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int WIDTH = 0;
int WIDTH2 = 0;
int HEIGHT = 0;

void read_file(const char *filename, char ***map, char **instructions) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    char line[1024];

    // read map until just an empty line...
    // get size...
    WIDTH = 0;
    HEIGHT = 0;
    while(fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") == 0)
            break;
        if (WIDTH == 0) WIDTH = strlen(line) - 1;
        HEIGHT += 1;
    };
    // get size of instruction set... (plus a few newlines)
    int file_pos = ftell(file);
    fseek(file, 0, SEEK_END);
    int instr_size = ftell(file) - file_pos;
    rewind(file);

    // allocate memory
    *map = (char**)malloc((HEIGHT + 1 ) * sizeof(char*));
    for (int i = 0; i < HEIGHT; i++) {
        (*map)[i] = (char*)malloc((WIDTH + 1) * sizeof(char));
    }
    *instructions = (char*)malloc((instr_size + 1) * sizeof(char));

    // Get map
    int i = 0;
    while(fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") == 0)
            break;
        strncpy((*map)[i], line, WIDTH);
        i++;
    }

    // get instructions
    char ch;
    int offset;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n')
            continue;
        if (ch == '<' || ch == '>' || ch == '^' || ch == 'v')
            (*instructions)[offset++] = ch;
    }

    fclose(file);

}

void free_data(char **map, char **second_map, char *instructions) {
    for (int i = 0; i < HEIGHT; i++) {
        free(map[i]);
    }
    free(map);
    for (int i = 0; i < HEIGHT; i++) {
        free(second_map[i]);
    }
    free(second_map);
    free(instructions);
}

typedef struct {
    int x;
    int y;
} XY;

XY move_up(char **map, XY r) {
    int ty = r.y;
    for (int y = r.y - 1; y > 0; y--) {
        if (map[y][r.x] == '#') break;
        if (map[y][r.x] == '.') {
            ty = y;
            break;
        }
    }
    if (ty == r.y)
        return r;

    for (int y = ty; y < r.y; y++)
        map[y][r.x] = map[y+1][r.x];
    map[r.y][r.x] = '.';

    r.y -= 1;
    return r;
}

XY move_down(char **map, XY r) {
    int ty = r.y;
    for (int y = r.y + 1; y < HEIGHT; y++) {
        if (map[y][r.x] == '#') break;
        if (map[y][r.x] == '.') {
            ty = y;
            break;
        }
    }
    if (ty == r.y)
        return r;

    for (int y = ty; y > r.y; y--)
        map[y][r.x] = map[y-1][r.x];
    map[r.y][r.x] = '.';

    r.y += 1;
    return r;
}

XY move_left(char **map, XY r) {
    int tx = r.x;
    for (int x = r.x - 1; x > 0; x--) {
        if (map[r.y][x] == '#') break;
        if (map[r.y][x] == '.') {
            tx = x;
            break;
        }
    }
    if (tx == r.x)
        return r;

    for (int x = tx; x < r.x; x++)
        map[r.y][x] = map[r.y][x+1];
    map[r.y][r.x] = '.';

    r.x -= 1;
    return r;
}

XY move_right(char **map, XY r) {
    int tx = r.x;
    for (int x = r.x + 1; x < WIDTH; x++) {
        if (map[r.y][x] == '#') break;
        if (map[r.y][x] == '.') {
            tx = x;
            break;
        }
    }
    if (tx == r.x)
        return r;

    for (int x = tx; x > r.x; x--)
        map[r.y][x] = map[r.y][x-1];
    map[r.y][r.x] = '.';

    r.x += 1;
    return r;
}

void print_map(char **map) {
    for (int y = 0; y < HEIGHT; y++)
        printf("%s\n", map[y]);
}

long part_1(char **map, char *instr) {

    // Where do we begin?
    XY r = {0,0};
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (map[y][x] == '@') {
                r = (XY){x,y};
                break;
            }
        }
        if (r.x != 0)
            break;
    }

    int step = 0;
    char d = ' ';
    while(d = instr[step++]) {
        if (d == '^')
            r = move_up(map, r);
        else if (d == 'v')
            r = move_down(map, r);
        else if (d == '<')
            r = move_left(map, r);
        else if (d == '>')
            r = move_right(map, r);
    }

    long sum_gps = 0;

    for (int y = 1; y < HEIGHT - 1; y++)
        for (int x = 1; x < WIDTH - 1; x++)
            if (map[y][x] == 'O')
                sum_gps += 100 * y + x;

    return sum_gps;
}

char **get_second_map(char **map) {
    WIDTH2 = WIDTH * 2;
    // allocate memory
    char **second = (char**)malloc((HEIGHT + 1 ) * sizeof(char*));
    for (int i = 0; i < HEIGHT; i++) {
        second[i] = (char*)malloc((WIDTH2 + 1) * sizeof(char));
    }

    // copy and adjust
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (map[y][x] == '#') {
                second[y][x * 2] = '#';
                second[y][x * 2 + 1] = '#';
            }
            else if (map[y][x] == 'O') {
                second[y][x * 2] = '[';
                second[y][x * 2 + 1] = ']';
            }
            else if (map[y][x] == '.') {
                second[y][x * 2] = '.';
                second[y][x * 2 + 1] = '.';
            }
            else if (map[y][x] == '@') {
                second[y][x * 2] = '@';
                second[y][x * 2 + 1] = '.';
            }
        }
    }

    return second;
}

int can_move_y_2(char **map, XY c, const int dy) {
    if (map[c.y + dy][c.x] == '#')
        return 0;

    if (map[c.y + dy][c.x] == '.')
        return 1;

    if (map[c.y + dy][c.x] == '[')
        return can_move_y_2(map, (XY){c.x, c.y + dy}, dy) && can_move_y_2(map, (XY){c.x + 1, c.y + dy}, dy);

    if (map[c.y + dy][c.x] == ']')
        return can_move_y_2(map, (XY){c.x, c.y + dy}, dy) && can_move_y_2(map, (XY){c.x - 1, c.y + dy}, dy);

    return 0;
}

int can_move_x_2(char **map, XY c, const int dx) {
    if (map[c.y][c.x + dx] == '#')
        return 0;

    if (map[c.y][c.x + dx] == '.')
        return 1;

    return can_move_x_2(map, (XY){c.x + dx, c.y}, dx);
}

void do_move_y_2(char **map, XY c, const int dy) {
    if (map[c.y + dy][c.x] == '.') {
        map[c.y + dy][c.x] = map[c.y][c.x];
        map[c.y][c.x] = '.';
    }
    else if (map[c.y + dy][c.x] == '[') {
        do_move_y_2(map, (XY){c.x, c.y + dy}, dy);
        do_move_y_2(map, (XY){c.x + 1, c.y + dy}, dy);
        do_move_y_2(map, c, dy);
    }
    else if (map[c.y + dy][c.x] == ']') {
        do_move_y_2(map, (XY){c.x, c.y + dy}, dy);
        do_move_y_2(map, (XY){c.x - 1, c.y + dy}, dy);
        do_move_y_2(map, c, dy);
    }
}

void do_move_x_2(char **map, XY c, const int dx) {
    if (map[c.y][c.x + dx] == '.') {
        map[c.y][c.x + dx] = map[c.y][c.x];
        map[c.y][c.x] = '.';
    }
    else {
        do_move_x_2(map, (XY){c.x + dx, c.y}, dx);
        do_move_x_2(map, c, dx);
    }
}

long part_2(char **map, char *instr) {

    // Where do we begin?
    XY r = {0,0};
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH2; x++) {
            if (map[y][x] == '@') {
                r = (XY){x,y};
                break;
            }
        }
        if (r.x != 0)
            break;
    }

    int step = 0;
    char d = ' ';
    while(d = instr[step++]) {
        if (d == '^') {
            if (can_move_y_2(map, r, -1)) {
                do_move_y_2(map, r, -1);
                r.y -= 1;
            }
        }
        else if (d == 'v') {
            if (can_move_y_2(map, r, 1)) {
                do_move_y_2(map, r, 1);
                r.y += 1;
            }
        }
        else if (d == '<') {
            if (can_move_x_2(map, r, -1)) {
                do_move_x_2(map, r, -1);
                r.x -= 1;
            }
        }
        else if (d == '>') {
            if (can_move_x_2(map, r, 1)) {
                do_move_x_2(map, r, 1);
                r.x += 1;
            }
        }
    }

    long sum_gps = 0;
    for (int y = 1; y < HEIGHT - 1; y++)
        for (int x = 1; x < WIDTH2 - 1; x++)
            if (map[y][x] == '[')
                sum_gps += 100 * y + x;

    return sum_gps;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    char **map = NULL;
    char *instructions = NULL;

    read_file(argv[1], &map, &instructions);

    char **second = get_second_map(map);

    long result1 = part_1(map, instructions);
    printf("part 1: %ld\n", result1);

    long result2 = part_2(second, instructions);
    printf("part 2: %ld\n", result2);

    free_data(map, second, instructions);
}