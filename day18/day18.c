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

XY BYTES[4096]; // Let's keep it simple :) 
int WIDTH = 0;
int HEIGHT = 0;
int BLOCKED[71][71];   // max size
int NUM_BYTES = 0;

const int dy[] = {-1, 1, 0, 0}; // u, d, l, r
const int dx[] = {0, 0, -1, 1}; // u, d, l, r

void read_bytes(const char *filename) {

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    // init bytes
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            BLOCKED[y][x] = INT_MAX;

    int t = 1;
    int x = 0;
    int y = 0;
    while(fscanf(file, "%d,%d", &x, &y) == 2) {
        BLOCKED[y][x] = t++;
        BYTES[++NUM_BYTES] = (XY){x, y};
    }

    fclose(file);
}

void print_map(const int time) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char c = BLOCKED[y][x] <= time ? '#' : '.';
            printf("%c", c);
        }
        printf("\n");
    }
}

int findMinDistance(Node *queue, bool* visited, int size) {
    int minDist = INT_MAX;
    int minIndex = -1;
    for (int i = 0; i < size; i++) {
        if (!visited[i] && queue[i].dist < minDist) {
            minDist = queue[i].dist;
            minIndex = i;
        }
    }
    return minIndex;
}

bool isVisitable(const XY xy, const int time) {
    return xy.x >= 0 && xy.y >= 0 && xy.x < WIDTH && xy.y < HEIGHT && BLOCKED[xy.y][xy.x] > time;
}

Node *queue = NULL;
bool *visited = NULL;
bool *in_a_path = NULL;

int dijkstra_1(XY start, XY end, int time) {
    int size = HEIGHT * WIDTH;

    int dist[size];
    XY prev[size];

    // initialize
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int i = y * WIDTH + x;
            dist[i] = INT_MAX;
            queue[i].pos = (XY){x, y};
            queue[i].dist = INT_MAX;
            prev[i] = (XY){-1, -1};
            visited[i] = false;
            in_a_path[i] = false;
        }
    }

    int startI = start.y * WIDTH + start.x;
    dist[startI] = 0;
    queue[startI].dist = 0;

    for (int go = 0; go < size; go++) {
        int u = findMinDistance(queue, visited, size);
        if (u == -1)
            break;

        visited[u] = true;
        XY current = queue[u].pos;

        for (int d = 0; d < 4; d++) {
            XY next = (XY){current.x + dx[d], current.y + dy[d]};

            if (isVisitable(next, time)) {
                int ni = next.y * WIDTH + next.x;
                int cost = dist[u] + 1;
                if (cost < dist[ni]) {
                    dist[ni] = cost;
                    queue[ni].dist = cost;
                    prev[ni] = current;
                }
            }
        }
    }

    // Make a path:
    in_a_path[end.y * WIDTH + end.x] = true;
    XY pi = prev[end.y * WIDTH + end.x];
    while (pi.x > -1) {
        in_a_path[pi.y * WIDTH + pi.x] = true;
        pi = prev[pi.y * WIDTH + pi.x];
    }

    return dist[end.y * WIDTH + end.x];
}

XY part_2(const int start_time) {
    int t = start_time + 1;
    for (; t < NUM_BYTES; t++) {
        XY byte = BYTES[t];
        if (in_a_path[byte.y * WIDTH + byte.x]) {
            if (dijkstra_1((XY){0,0}, (XY){WIDTH - 1, HEIGHT - 1}, t) == INT_MAX)
                break;
        }
    }

    return BYTES[t];

    XY byte = {-1, -1};

    // find the byte
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (BLOCKED[y][x] == t) {
                byte = (XY){x, y};
                return byte;
            }
        }
    }

    return byte;
}

int main(int argc, char **argv)
{
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Usage: %s <input> [test]\n", argv[0]);
        exit(1);
    }

    int time = 0;

    if (argc >= 3 && strcmp(argv[2], "test") == 0) {
        WIDTH = 7;
        HEIGHT = 7;
        time = 12;
    } else {
        WIDTH = 71;
        HEIGHT = 71;
        time = 1024;
    }

    read_bytes(argv[1]);

    queue = (Node*)malloc(HEIGHT * WIDTH * sizeof(Node));
    visited = (bool*)calloc(HEIGHT * WIDTH, sizeof(bool));
    in_a_path = (bool*)calloc(HEIGHT * WIDTH, sizeof(bool));

    int result1 = dijkstra_1((XY){0,0}, (XY){WIDTH - 1, HEIGHT - 1}, time);
    printf("part 1: %d\n", result1);

    XY result2 = part_2(time);
    printf("part 2: %d,%d\n", result2.x, result2.y);

    free(queue);
    free(visited);

    //print_map(time);
    return 0;
}