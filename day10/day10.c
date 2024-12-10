#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int **read_map(const char *filename, int *height, int *width)
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
    int **lines = malloc(*height * sizeof(int *));
    if (lines == NULL)
    {
        fprintf(stderr, "Error allocating memory for lines buffer\n");
        fclose(file);
        exit(1);
    }

    // allocate memory for all of the lines
    for (int i = 0; i < *height; i++)
    {
        lines[i] = malloc((*width + 1) * sizeof(int));
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
            lines[i][j] = buffer[j] - '0';
        }
    }

    fclose(file);
    return lines;
}

void free_map(int **map, int height)
{
    // return all the memory :)
    for (int i = 0; i < height; i++)
    {
        free(map[i]);
    }
    free(map);
}

// Making a hash table kind of thing?
typedef struct {
    int sx;
    int sy;
    int tx;
    int ty;
} Coordinate;

typedef struct CNode {
    Coordinate coord;
    struct CNode* next;
} CNode;

#define HASH_SIZE 1000
CNode *cHashTable[HASH_SIZE] = {NULL};

int cHash(Coordinate coord) {
    return (coord.sx + coord.sy) % HASH_SIZE;
}

void cInsert(Coordinate coord) {
    int index = cHash(coord);
    CNode *newNode = (CNode *)malloc(sizeof(CNode));
    newNode->coord = coord;
    newNode->next = cHashTable[index];
    cHashTable[index] = newNode;
}

int cSearch(Coordinate coord) {
    int index = cHash(coord);
    CNode *current = cHashTable[index];
    while (current != NULL) {
        if (current->coord.sx == coord.sx && current->coord.sy == coord.sy && current->coord.tx == coord.tx && current->coord.ty == coord.ty) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int cCount(Coordinate coord) {
    int index = cHash(coord);
    int count = 0;
    CNode *current = cHashTable[index];
    while (current != NULL) {
        if (current->coord.sx == coord.sx && current->coord.sy == coord.sy) {
            count ++;
        }
        current = current->next;
    }
    return count;

}

void freeHashTable() {
    for (int i = 0; i < HASH_SIZE; i++) {
        CNode *current = cHashTable[i];
        while (current != NULL) {
            CNode *next = current->next;
            free(current);
            current = next;
        }
    }
}

int count_paths(int **map, const int height, const int width, const int x, const int y, const int h, const int sx, const int sy) {
    if (y < 0 || y >= height || x < 0 || x >= width || map[y][x] != h)
        return 0;

    int count = 0;
    if (h == 9) {
        Coordinate c = {sx, sy, x, y};
        if (!cSearch(c)) {
            cInsert(c);
        }
        return 1;
    }
    if (y > 0) {
        count += count_paths(map, height, width, x, y - 1, h + 1, sx, sy);
    }
    if (x < width - 1) {
        count += count_paths(map, height, width, x + 1, y, h + 1, sx, sy);
    }
    if (y < height - 1) {
        count += count_paths(map, height, width, x, y + 1, h + 1, sx, sy);
    }
    if (x > 0) {
        count += count_paths(map, height, width, x - 1, y, h + 1, sx, sy);
    }
    return count;
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    int height = 0;
    int width = 0;
    int **map = read_map(argv[1], &height, &width);

    // When you by mistake implement part 2 first, realize that's wrong but
    // keeps the old stuff and just add to it to actually solve part 1.
    // Then part2 is really easy to just sum the first counter you did :D

    int sum1 = 0;
    int sum2 = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == 0) {
                int count2 = 0;
                count2 += count_paths(map, height, width, x, y - 1, 1, x, y);
                count2 += count_paths(map, height, width, x + 1, y, 1, x, y);
                count2 += count_paths(map, height, width, x, y + 1, 1, x, y);
                count2 += count_paths(map, height, width, x - 1, y, 1, x, y);

                Coordinate c = {x,y};
                int count1 = cCount(c);

                sum1 += count1;
                sum2 += count2;
            }
        }
    }

    freeHashTable();

    printf("part 1: %ld\n", sum1);
    printf("part 2: %ld\n", sum2);

    free_map(map, height);

    return 0;
}