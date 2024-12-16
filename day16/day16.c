#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

typedef struct Node {
    int x;
    int y;
} Node;

typedef struct Vertex {
    Node a;
    Node b;
    int c;
    char d;
    int i;
} Vertex;

typedef struct HNode {
    Vertex v;
    struct HNode *next;
} HNode;

#define HASH_SIZE 1000
HNode *hashTable[HASH_SIZE] = {NULL};

int hash(Node n) {
    return n.x * n.y % HASH_SIZE;
}

int vertexCount = 0;

HNode *hash_insert(Vertex v) {
    int index = hash(v.a);
    HNode *newNode = (HNode *)malloc(sizeof(HNode));
    newNode->v = v;
    newNode->v.i = vertexCount++;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
    return newNode;
}

typedef struct {
    Vertex *v[5];
} VertexArray;

VertexArray hash_find_vertices(Node n) {
    VertexArray result = { {NULL, NULL, NULL, NULL, NULL} }; // can always be max 4
    int index = hash(n);
    HNode *current = hashTable[index];
    int vi = 0;
    while (current != NULL) {
        if (current->v.a.x == n.x && current->v.a.y == n.y) {
            result.v[vi++] = &current->v;
        }
        current = current->next;
    }
    result.v[vi] = NULL;    // Safety dance
    return result;
}

void free_vertex_array(Vertex **v) {
    free(v);
}

void freeHashTable() {
    for (int i = 0; i < HASH_SIZE; i++) {
        HNode *current = hashTable[i];
        while (current != NULL) {
            HNode *next = current->next;
            free(current);
            current = next;
        }
    }
}

Node startNode = {0, 0};
Node endNode = {0, 0};

void find_start_end_nodes(char **map, const int width, const int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == 'S')
                startNode = (Node){x, y};
            
            if (map[y][x] == 'E')
                endNode = (Node){x, y};
        }
    }
}

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

    find_start_end_nodes(lines, *width, *height);

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


void get_vertices(char ** map, const int width, const int height) {

    // horizontal
    for (int y = 1; y < height; y += 2) {
        int px = 0;
        for (int x = 1; x < width; x++) {
            if (map[y][x] == '#') {
                px = 0;
                continue;
            }
            if (map[y][x] == 'S' || map[y][x] == 'E' || ( map[y][x] == '.' && ( map[y-1][x] == '.' || map[y+1][x] == '.' ))) {
                if (!px)
                    px = x;
                else {
                    hash_insert((Vertex){{px, y}, {x, y}, x - px, 'x'});
                    hash_insert((Vertex){{x, y},{px, y}, x - px, 'x'});
                    px = x;
                }
            }
        }
    }

    // vertical
    for (int x = 1; x < width; x += 2) {
        int py = 0;
        for (int y = 1; y < height; y++) {
            if (map[y][x] == '#') {
                py = 0;
                continue;
            }
            if (map[y][x] == 'S' || map[y][x] == 'E' || ( map[y][x] == '.' && ( map[y][x - 1] == '.' || map[y][x + 1] == '.' ))) {
                if (!py)
                    py = y;
                else {
                    hash_insert((Vertex){{x, py}, {x, y}, y - py, 'y'});
                    hash_insert((Vertex){{x, y},{x, py}, y - py, 'y'});
                    py = y;
                }
            }
        }
    }
}

int minDistance(int dist[], bool sptSet[]) {
    int min = INT_MAX;
    int min_index;
    for (int v = 0; v < vertexCount; v++) {
        if (sptSet[v] == false && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

void printSolution(int dist[]) {
    printf("Vertex \t Distance from Source\n");
    for (int i = 0; i < vertexCount; i++)
        printf("%d \t\t %d\n", i, dist[i]);
}

long dijkstra(Node start) {

    int n = 0;
    Vertex vertices[vertexCount];
    int dist[vertexCount];
    int prev[vertexCount];
    char dir[vertexCount];
    bool sptSet[vertexCount];

    for (int i = 0; i < vertexCount; i++) {
        dist[i] = INT_MAX;
        dir[i] = ' ';
        prev[i] = -1;
        sptSet[i] = false;
    }

    for (int i = 0; i < HASH_SIZE; i++) {
        HNode *current = hashTable[i];
        while (current != NULL) {
            n = current->v.i;
            vertices[n] = current->v;
            if (current->v.a.x == start.x && current->v.a.y == start.y) {
                if (current->v.d == 'x')
                    dist[n] = current->v.c;
                else
                    dist[n] = 1000 + current->v.c;
                dir[n] = current->v.d;
            }

            current = current->next;
        }
    }

    for (int i = 0; i < vertexCount; i++) {
        int u = minDistance(dist, sptSet);
        sptSet[u] = true;
        VertexArray b = hash_find_vertices(vertices[u].b);
        for (int bi = 0; b.v[bi] != NULL; bi++) {
            int cost = b.v[bi]->d == dir[u] ? b.v[bi]->c : 1000 + b.v[bi]->c;
            if (dist[u] + cost < dist[b.v[bi]->i]) {
                dist[b.v[bi]->i] = dist[u] + cost;
                dir[b.v[bi]->i] = b.v[bi]->d;
                prev[b.v[bi]->i] = u;
            }
        }
    }

    int min_cost = INT_MAX;
    int min_vid = -1;
    for (int i = 0; i < HASH_SIZE; i++) {
        HNode *current = hashTable[i];
        while (current != NULL) {
            if (current->v.b.x == endNode.x && current->v.b.y == endNode.y) {
                printf("%d\n",dist[current->v.i]);
                if (dist[current->v.i] < min_cost) {
                    min_cost = dist[current->v.i];
                    min_vid = current->v.i;
                }
            }
            current = current->next;
        }
    }

    return min_cost;
}



int part_2(int goal) {

    return 0;
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

    get_vertices(map, width, height);

    int result1 = dijkstra(startNode);

    int result2 = part_2(result1);


    //long result1 = part_1(width, height, plot, pid_count);
    printf("part 1: %d\n", result1);

    //long result2 = part_2(width, height, plot, pid_count);
    printf("part 2: %ld\n", result2);

    free_map(map, height);
    freeHashTable();
    return 0;
}

