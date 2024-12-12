#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **read_map(const char *filename, int *height, int *width)
{
    // Open file
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    // Read one line to determine length (in this case, all lines are equal length)
    char buffer_width[1024];
    int file_width = 0;
    if (fgets(buffer_width, 1024, file))
    {
        file_width = strlen(buffer_width);
        *width = file_width - 1; // do not include end-of-line character
    }
    else
    {
        fprintf(stderr, "Could not get line length from file\n");
        fclose(file);
        exit(1);
    }

    // Go to the end of file to determine total file size
    // Number of lines = total size / line length
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file) + 1;
    *height = file_size / file_width;
    if (*height < 1)
    {
        fprintf(stderr, "Could not get number of lines from file\n");
        fclose(file);
        exit(1);
    }

    // We want to start reading again at the beginning of the file
    rewind(file);

    // Allocate memory for lines buffer
    char **lines = malloc(*height * sizeof(char *));
    if (lines == NULL)
    {
        fprintf(stderr, "Error allocating memory for lines buffer\n");
        fclose(file);
        exit(1);
    }

    // allocate memory for all of the lines
    for (int i = 0; i < *height; i++)
    {
        lines[i] = malloc((*width + 1) * sizeof(char));
        if (lines[i] == NULL)
        {
            fprintf(stderr, "Error allocating memory for line index %d\n", i);
            for (int j = 0; j < i; j++)
            {
                free(lines[j]);
            }
            free(lines);
            fclose(file);
            exit(1);
        }
    }

    // read all of the lines into lines buffer
    for (int i = 0; i < *height; i++)
    {
        char buffer[*width + 2];
        if (fgets(buffer, *width + 2, file) == NULL)
        {
            fprintf(stderr, "Error reading file line %d\n", i + 1);
            for (int j = 0; j < 1; j++)
            {
                free(lines[j]);
            }
            free(lines);
            fclose(file);
            exit(1);
        }
        for (int j = 0; j < *width; j++) {
            lines[i][j] = buffer[j];
        }
    }

    fclose(file);
    return lines;
}

void free_map(char **map, int height)
{
    // return all the memory :)
    for (int i = 0; i < height; i++)
    {
        free(map[i]);
    }
    free(map);
}

typedef struct {
    int x;
    int y;
} Coords;

typedef struct {
    Coords m;
    int pid;
    char c;
    int b;
} Plot;

void map_to_plot(char **map, int width, int height, Plot plot[height][width], int *pid_count) {
    // Identify regions
    int pid = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Plot *p = &plot[y][x];
            p->m = (Coords){x, y}; 
            p->c = map[y][x];

            if (y > 0 && map[y - 1][x] == p->c)
                p->pid = plot[y - 1][x].pid;
            else if (x > 0 && map[y][x - 1] == p->c)
                p->pid = plot[y][x - 1].pid;
            else
                p->pid = pid++;

            p->b = (y == 0          || map[y - 1][x] != p->c)
                 + (y == height - 1 || map[y + 1][x] != p->c)
                 + (x == 0          || map[y][x - 1] != p->c) 
                 + (x == width - 1  || map[y][x + 1] != p->c);

        }
    }

    *pid_count = pid;

    // above is not very exact, so join adjacent regions here
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Plot *p = &plot[y][x];
            for (int n = 0; n < 4; n++) {
                int dx = 0;
                int dy = 0;
                if (n == 0) {
                    if ( y == 0) continue;
                    dy = -1;
                }
                else if (n == 1) {
                    if (x == width - 1) continue;          
                    dx = 1;
                }
                else if (n == 2) {
                    if (y == height - 1) continue;
                    dy = 1;
                }
                else {
                    if (x == 0) continue;
                    dx = -1;
                }

                if (plot[y + dy][x + dx].c == p->c && plot[y + dy][x + dx].pid != p->pid) {
                    int join_pid = plot[y + dy][x + dx].pid;
                    for(int jy = 0; jy < height; jy++) {
                        for (int jx = 0; jx < width; jx++) {
                            if (plot[jy][jx].pid == join_pid)
                                plot[jy][jx].pid = p->pid;
                        }
                    }
                }
            }
        }
    }
}

long part_1(int width, int height, Plot plot[height][width], int pid_count) {
    long price = 0;

    // Bah, just do the thing, brute-force style :D
    for (int pid = 0; pid < pid_count; pid++) {
        long area = 0;
        long perimeter = 0;
        char crop = ' ';
        // Yeah, this could totally be optimized but what the hey, it's enough
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (plot[y][x].pid != pid)
                    continue;
                area += 1;
                perimeter += plot[y][x].b;
                crop = plot[y][x].c;
            }
        }       
        price += area * perimeter;
    }

    return price;
}

typedef struct {
    int pid;
    long u;
    long r;
    long d;
    long l;
    long a;
} Border;

long part_2(int width, int height, Plot plot[height][width], int pid_count) {
    Border borders[pid_count];

    // Ok, let's just do it again and not be clever... :)
    // Counting up and down borders, left-right and
    // left and right borders, up-down
    //
    // This works, could be cuter, I wont bother  

    for (int pid = 0; pid < pid_count; pid++) {
        Border *b = &borders[pid];
        b->a = 0;
        int in;
        int pid_cmp;

        // Up
        b->u = 0;
        for (int y = 0; y < height; y++) {
            in = 0;
            for (int x = 0; x < width; x++) {
                if (y == 0)
                    pid_cmp = -1;
                else
                    pid_cmp = plot[y - 1][x].pid;

                int pid_this = plot[y][x].pid;
                if (pid_this == pid) b->a += 1;

                if (pid_this != pid && !in)
                    continue;
                else if (pid_this == pid && pid_cmp != pid && !in) {
                    in = 1;
                    b->u += 1;
                }
                else if (pid_this == pid && pid_cmp == pid && in)
                    in = 0;
                else if (pid_this != pid && in)
                    in = 0;
            }
        }

        // Down
        b->d = 0;
        for (int y = 0; y < height; y++) {
            in = 0;
            for (int x = 0; x < width; x++) {
                if (y == height - 1)
                    pid_cmp = -1;
                else
                    pid_cmp = plot[y + 1][x].pid;

                int pid_this = plot[y][x].pid;

                if (pid_this != pid && !in)
                    continue;
                else if (pid_this == pid && pid_cmp != pid && !in) {
                    in = 1;
                    b->d += 1;
                }
                else if (pid_this == pid && pid_cmp == pid && in)
                    in = 0;
                else if (pid_this != pid && in)
                    in = 0;
            }
        }

        // Right
        b->r = 0;
        for (int x = 0; x < width; x++) {
            in = 0;
            for (int y = 0; y < height; y++) {
                if (x == width - 1)
                    pid_cmp = -1;
                else
                    pid_cmp = plot[y][x + 1].pid;
                    
                int pid_this = plot[y][x].pid;

                if (pid_this != pid && !in)
                    continue;
                else if (pid_this == pid && pid_cmp != pid && !in) {
                    in = 1;
                    b->r += 1;
                }
                else if (pid_this == pid && pid_cmp == pid && in)
                    in = 0;
                else if (pid_this != pid && in)
                    in = 0;
            }
        }

        // Left
        b->l = 0;
        for (int x = 0; x < width; x++) {
            in = 0;
            for (int y = 0; y < height; y++) {
                if (x == 0)
                    pid_cmp = -1;
                else
                    pid_cmp = plot[y][x - 1].pid;

                int pid_this = plot[y][x].pid;
                if (pid_this != pid && !in)
                    continue;
                else if (pid_this == pid && pid_cmp != pid && !in) {
                    in = 1;
                    b->l += 1;
                }
                else if (pid_this == pid && pid_cmp == pid && in)
                    in = 0;
                else if (pid_this != pid && in)
                    in = 0;
            }
        }
    }


    long price = 0;

    for (int i = 0; i < pid_count; i++) {
        Border *b = &borders[i];

        long sides = b->u + b->r + b->d + b->l;
        if (sides == 0)
            continue;

        price += b->a * sides;
    }

    return price;
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    int height = 0;
    int width = 0;
    char **map = read_map(argv[1], &height, &width);

    Plot plot[height][width];
    int pid_count = 0;
    map_to_plot(map, width, height, plot, &pid_count);

    long result1 = part_1(width, height, plot, pid_count);
    printf("part 1: %ld\n", result1);

    long result2 = part_2(width, height, plot, pid_count);
    printf("part 2: %ld\n", result2);

    free_map(map, height);
    return 0;
}
