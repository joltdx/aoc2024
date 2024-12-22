#include <stdio.h>
#include <stdlib.h>

void read_initial_secrets(const char *filename, int **secrets, int *num_secrets) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    // number of secrets
    char line[16];
    while (fgets(line, sizeof(line), file))
        *num_secrets += 1;
    if (line[0] == '\n')
        *num_secrets -= 1;

    rewind(file);
    *secrets = (int*)malloc((*num_secrets)*sizeof(int));

    int i = 0;
    while(fscanf(file, "%d", &(*secrets)[i++]) == 1);

    fclose(file);
}

long pseudorandom(const long in, const int steps) {
    long out = in;

    for (int i = 0; i < steps; i++) {
        out = out ^ ( out * 64 ) % 16777216;

        out = out ^ ( out / 32 ) % 16777216;

        out = out ^ ( out * 2048 ) % 16777216;
    }
    return out;
}

long part_1(int *secrets, const int num_secrets) {
    long sum = 0;

    for (int i = 0; i < num_secrets; i++) {
        long secret = secrets[i];
        sum += pseudorandom(secrets[i], 2000);
    }

    return sum;
}

typedef struct {
    int seq[4];
    int price;
} Diff;

typedef struct DNode {
    Diff diff;
    struct DNode *next;
} DNode;

#define HASH_SIZE 3037
#define MAX_INPUT 1700
DNode *diffTable[MAX_INPUT][HASH_SIZE] = {NULL};

int hash(int seq[4]) {
    unsigned int hash = 0;
    hash = (hash + ((unsigned int)(seq[0] + 9) << 16)) * 17;
    hash = (hash + ((unsigned int)(seq[1] + 9) << 12)) * 23;
    hash = (hash + ((unsigned int)(seq[2] + 9) << 8)) * 31;
    hash = (hash + ((unsigned int)(seq[3] + 9) << 4)) * 37;
    
    return hash % HASH_SIZE;
}

void diff_insert(Diff diff, int buyer) {
    int index = hash(diff.seq);
    DNode *newNode = (DNode*)malloc(sizeof(DNode));
    newNode->diff = diff;
    newNode->next = diffTable[buyer][index];
    diffTable[buyer][index] = newNode;
}

DNode *diff_find(int seq[4], int buyer) {
    int index = hash(seq);
    DNode *current = diffTable[buyer][index];
    while (current != NULL) {
        if (current->diff.seq[0] == seq[0] &&
            current->diff.seq[1] == seq[1] &&
            current->diff.seq[2] == seq[2] &&
            current->diff.seq[3] == seq[3] ) {

            return current;
        }
        current = current->next;
    }
    return NULL;
}

long part_2(int *secrets, const int num_secrets) {
    int seq[4] = {0,0,0,0};

    for (int i = 0; i < num_secrets; i++) {
        long secret = secrets[i];
        long prev = secret % 10;
        for (int n = 0; n < 3; n++) {
            secret = pseudorandom(secret, 1);
            int price = secret % 10;
            seq[n + 1] = price - prev;
            prev = price;
        }

        for (int n = 3; n < 2000; n++) {
            secret = pseudorandom(secret, 1);
            int price = secret % 10;
            seq[0] = seq[1];
            seq[1] = seq[2];
            seq[2] = seq[3];
            seq[3] = price - prev;
            prev = price;
            if (diff_find(seq, i) == NULL) {
                Diff diff;
                diff.seq[0] = seq[0];
                diff.seq[1] = seq[1];
                diff.seq[2] = seq[2];
                diff.seq[3] = seq[3];
                diff.price = price;
                diff_insert(diff, i);
            }
        }

    }

    long max_sum = 0;
    long sum = 0;
    DNode *diff;

    for (int a = -9; a < 10; a++) {
        for (int b = -9; b < 10; b++) {
            for (int c = -9; c < 10; c++) {
                for (int d = -9; d < 10; d++) {
                    int seq[4] = {a, b, c, d};
                    sum = 0;
                    for (int i = 0; i < num_secrets; i++) {
                        diff = diff_find(seq, i);
                        if (diff != NULL)
                            sum += diff->diff.price;
                    }
                    if (sum > max_sum)
                        max_sum = sum;
                }
            }
        }
    }

    for (int i = 0; i < MAX_INPUT; i++) {
        for (int j = 0; j < HASH_SIZE; j++) {
            DNode *current = diffTable[i][j];
            while (current != NULL) {
                DNode *next = current->next;
                free(current);
                current = next;
            }
        }
    }

    return max_sum;
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    int num_secrets = 0;
    int *secrets;
    read_initial_secrets(argv[1], &secrets, &num_secrets);

    long result1 = part_1(secrets, num_secrets);
    printf("part 1: %ld\n", result1);

    printf("(wait for it, part 2 brute force in progress)\n");
    long result2 = part_2(secrets, num_secrets);
    printf("part 2: %ld\n", result2);

    free(secrets);

    return 0;
}