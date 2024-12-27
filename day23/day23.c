#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Comp {
    char id[2];
    int links[32];    // let's just...
    int l_count;
} Comp;

Comp comp[26*26] = {0};

int comp2[26*26] = {-1};

int get_comp_index(char *id) {
    return ( id[0] - 'a' ) * 26 + ( id[1] - 'a' );
}
char id_buffer[3] = {0};
char *get_comp_id(int index) {
    id_buffer[0] = (index / 26 ) + 'a';
    id_buffer[1] = (index % 26 ) + 'a';
    return id_buffer;
}

int is_connected(int a, int b) {
    for (int i = 0; i < comp[a].l_count; i++) {
        if (comp[a].links[i] == b)
            return 1;
    }
    return 0;
}

void read_comp(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    // initialize
    for (int a = 0; a < 26; a++) {
        for (int b = 0; b < 26; b++) {
            Comp newComp = {0};
            newComp.id[0] = 'a' + a;
            newComp.id[1] = 'a' + b; 
            comp[a * 26 + b] = newComp;
        }
    }


    char line[8];
    char a[2];
    char b[2];
    int index_a;
    int index_b;
    while(fgets(line, 8, file)) {
        if (line[0] == '\n')
            continue;
        a[0] = line[0];
        a[1] = line[1];
        b[0] = line[3];
        b[1] = line[4];
        index_a = get_comp_index(a);
        index_b = get_comp_index(b);

        comp[index_a].links[comp[index_a].l_count++] = index_b;
        comp[index_b].links[comp[index_b].l_count++] = index_a;
        if (index_a < index_b) {
            comp2[index_a] = index_b;
        } else {
            comp2[index_b] = index_a;
        }
    }
}

typedef struct SetOfThree {
    int c[3];
} SetOfThree;

SetOfThree sets[10000] = {-1};
int set_count = 0;

int find_set(int c[3]) {
    int i = 0;
    while (i < set_count && sets[i].c[0] != -1) {
        if (sets[i].c[0] == c[0] && sets[i].c[1] == c[1] && sets[i].c[2] == c[2])
            return 1;
        i++;
    }
    return 0;
}

void order_set(int c[3]) {
    if (c[0] > c[1]) {
        int temp = c[0];
        c[0] = c[1];
        c[1] = temp;
    }
    if (c[1] > c[2]) {
        int temp = c[1];
        c[1] = c[2];
        c[2] = temp;
    }
    if (c[0] > c[1]) {
        int temp = c[0];
        c[0] = c[1];
        c[1] = temp;
    }
}

void add_set(int c[3]) {
    sets[set_count].c[0] = c[0];
    sets[set_count].c[1] = c[1];
    sets[set_count].c[2] = c[2];
    set_count++;
}

int part_1() {

    int count = 0;

    // starts with t: between 't' * 26 + 0 and 't' * 26 + 25
    int t = 't' - 'a';
    for (int i = 0; i < 26*26; i++) {
        if (( i >= t * 26 && i <= t * 26 + 25 )) {
            int ic = comp[i].l_count;
            if ( ic > 0) {
                int *links = comp[i].links;

                for (int j = 0; j < ic; j++) {
                    int comp_j = links[j];
                    for (int k = j + 1; k < ic; k++) {
                        int comp_k = links[k];

                        if (is_connected(comp_j, comp_k)) {
                            int comps[3] = {i, comp_j, comp_k};
                            order_set(comps);
                            if (!find_set(comps)) {
                                count++;
                                add_set(comps);
                            }
                        }
                    }
                }
            }
        }
    }

    return count;
}


#define MAX_NODES 640

int nNodes = 0;

static int bestSize = 0;
static int bestClique[MAX_NODES];

void getNeighbors(int v, int *neighbors, int *count) {
    *count = comp[v].l_count;
    for (int i = 0; i < *count; i++) {
        neighbors[i] = comp[v].links[i];
    }
}

void BronKerbosch(int *R, int rSize, int *P, int pSize, int *X, int xSize) {
    if (pSize == 0 && xSize == 0) {
        // Found a maximal clique
        if (rSize > bestSize) {
            bestSize = rSize;
            memcpy(bestClique, R, rSize * sizeof(int));
        }
        return;
    }

    // Make a copy of P because we'll modify it
    int *Pcopy = (int *)malloc(pSize * sizeof(int));
    memcpy(Pcopy, P, pSize * sizeof(int));

    // For each vertex in P
    for (int i = 0; i < pSize; i++) {
        int v = Pcopy[i];

        // Find neighbors of v, to compute P ∩ N(v) and X ∩ N(v)
        int neighbors[MAX_NODES], nCount;
        getNeighbors(v, neighbors, &nCount);

        // Build P_intersect and X_intersect
        int P_next[MAX_NODES], pNextSize = 0;
        int X_next[MAX_NODES], xNextSize = 0;

        // P ∩ N(v)
        for (int j = 0; j < pSize; j++) {
            // check if P[j] is in neighbors
            int node = P[j];
            // if node is in neighbors
            for (int k = 0; k < nCount; k++) {
                if (neighbors[k] == node) {
                    P_next[pNextSize++] = node;
                    break;
                }
            }
        }

        // X ∩ N(v)
        for (int j = 0; j < xSize; j++) {
            int node = X[j];
            for (int k = 0; k < nCount; k++) {
                if (neighbors[k] == node) {
                    X_next[xNextSize++] = node;
                    break;
                }
            }
        }

        // R ∪ {v}
        int R_next[MAX_NODES];
        memcpy(R_next, R, rSize * sizeof(int));
        R_next[rSize] = v;

        BronKerbosch(R_next, rSize + 1,
                     P_next, pNextSize,
                     X_next, xNextSize);

        // Move v from P to X
        // We do that by removing v from P and adding to X
        for (int j = 0; j < pSize; j++) {
            if (P[j] == v) {
                P[j] = P[pSize - 1];
                pSize--;
                break;
            }
        }
        X[xSize++] = v;
    }

    free(Pcopy);
}

void part_2() {
    for (int i = 0; i < 26*26; i++) {
        if(comp[i].l_count > 0)
            nNodes++;
    }

    // Prepare the initial sets for Bron–Kerbosch:
    int R[MAX_NODES], rSize = 0;  // Start with empty R
    int P[MAX_NODES], pSize = 0;  // Initially P has all nodes
    int X[MAX_NODES], xSize = 0;  // Initially empty

    for (int i = 0; i < nNodes; i++) {
        P[i] = i;
    }

    pSize = 0;
    for (int i = 0; i < 26*26; i++) {
        if(comp[i].l_count > 0)
            P[pSize++] = i;
    }

    // run it
    BronKerbosch(R, rSize, P, pSize, X, xSize);

    // sort result
    for (int i = 0; i < bestSize - 1; i++) {
        for (int j = 0; j < bestSize - i - 1; j++) {
            if (bestClique[j] > bestClique[j + 1]) {
                int temp = bestClique[j];
                bestClique[j] = bestClique[j + 1];
                bestClique[j + 1] = temp;
            }
        } 
    }

    // output
    printf("part 2: ");
    for (int i = 0; i < bestSize; i++) {
        if (i > 0)
            printf(",");
        printf("%2s", get_comp_id(bestClique[i]));
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    read_comp(argv[1]);

    int result1 = part_1();
    printf("part 1: %d\n", result1);
    
    part_2();

}