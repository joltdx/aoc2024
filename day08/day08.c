#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int x;
    int y;
} Coordinates;

typedef struct {
    char f;
    int n;
    Coordinates c[32];
} Antenna;

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
        if (fgets(lines[i], *width + 2, file) == NULL)
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
        int len = strlen(lines[i]);
        if (len > 0 && lines[i][len - 1] == '\n')
        {
            lines[i][len - 1] = '\0'; // Remove newline character
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

char **copy_map(char **map, const int height, const int width) {
    char **copy = (char **)malloc(height * sizeof(char *));
    if (copy == NULL) {
        fprintf(stderr, "Error allocating memory for copy buffer\n");
            free_map(map, height);
            exit(1);
    }
    for (int i = 0; i < height; i++) {
        copy[i] = (char *)malloc(width * sizeof(char));
        if (copy[i] == NULL) {
            fprintf(stderr, "Error allocating memory for copy index %d\n", i);
            for (int j = 0; j < i; j++)
            {
                free(copy[j]);
            }
            free(copy);
            free_map(map, height);
            exit(1);   
        } 
        strcpy(copy[i], map[i]);
    }
    return copy;
}

int part_1(Antenna *ants, char **map, const int width, const int height) {
    int count = 0;

    char **antinodes = copy_map(map, height, width);

    int a = 0;
    while(ants[a].f) {
        for (int o = 0; o <= ants[a].n; o++) {
            int ox = ants[a].c[o].x;
            int oy = ants[a].c[o].y;
            for (int i = o + 1; i <= ants[a].n; i++) {
                int ix = ants[a].c[i].x;
                int iy = ants[a].c[i].y;
                int dx = ix - ox;
                int dy = iy - oy;
                if (ox - dx >= 0 && ox - dx < width && 
                    oy - dy >= 0 && oy - dy < height) {
                        antinodes[oy - dy][ox - dx] = '#';
                    }
                if (ix + dx >= 0 && ix + dx < width &&
                    iy + dy >= 0 && iy + dy < height) {
                        antinodes[iy + dy][ix + dx] = '#';
                    } 
            }
        }

        a++;
    }

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (antinodes[y][x] == '#')
                count++;

    free_map(antinodes, height);

    return count;
}

int part_2(Antenna *ants, char **map, const int width, const int height) {
    int count = 0;

    char **antinodes = copy_map(map, height, width);

    int a = 0;
    while(ants[a].f) {
        for (int o = 0; o <= ants[a].n; o++) {
            int ox = ants[a].c[o].x;
            int oy = ants[a].c[o].y;
            for (int i = o + 1; i <= ants[a].n; i++) {
                int ix = ants[a].c[i].x;
                int iy = ants[a].c[i].y;
                int dx = ix - ox;
                int dy = iy - oy;
                int n = 1;
                while (ox - dx * n >= 0 && ox - dx * n < width && 
                       oy - dy * n >= 0 && oy - dy * n < height) {
                        antinodes[oy - dy * n][ox - dx * n] = '#';
                        n++;
                    }
                n = 1;
                while (ix + dx * n >= 0 && ix + dx * n < width &&
                    iy + dy * n >= 0 && iy + dy * n < height) {
                        antinodes[iy + dy * n][ix + dx * n] = '#';
                        n++;
                    } 
                antinodes[oy][ox] = '#';
                antinodes[iy][ix] = '#';
            }
        }

        a++;
    }


    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (antinodes[y][x] == '#')
                count++;

    free_map(antinodes, height);

    return count;
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    int height;
    int width;

    char **map = read_map(argv[1], &height, &width);

    Antenna ants[150];

    int ac = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char c = map[y][x];
            if (c == '.') continue;

            int a = ac;
            int n = 0;
            for (; a >= 0; a--) {
                if (ants[a].f == c) {
                    n = ants[a].n + 1;
                    break;
                }
            }
            if (!n) {
                a = ac;
                ac++;
            }
            ants[a].f = c;
            ants[a].n = n;
            ants[a].c[n].x = x;
            ants[a].c[n].y = y;
        }
    }

    int result1 = part_1(ants, map, width, height);
    printf("part 1: %d\n", result1);

    int result2 = part_2(ants, map, width, height);
    printf("part 2: %d\n", result2);

    free_map(map, height);

    return 0;
}