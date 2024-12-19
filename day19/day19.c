#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char p[8];
    int l;
} Pat;

typedef struct {
    char d[64];
    int l;
} Des;

Des *designs;
int design_count = 0;

typedef struct HNode {
    Pat pat;
    struct HNode *next;
} HNode;

HNode *patTable[5] = {NULL};

int hash(char c) {
    //w u b r g
    switch (c) {
        case 'w':
            return 0;        
        case 'u':
            return 1;        
        case 'b':
            return 2;        
        case 'r':
            return 3;        
        default:
            return 4;
    }
}

HNode *patInsert(Pat pat) {
    int index = hash(pat.p[0]);
    HNode *newNode = (HNode *)malloc(sizeof(HNode));
    newNode->pat = pat;
    newNode->next = patTable[index];
    patTable[index] = newNode;
    return newNode;
}

bool isInPatTable(Pat pat) {
    int index = hash(pat.p[0]);
    HNode *current = patTable[index];
    while (current != NULL) {
        if (current->pat.l == pat.l && strncmp(current->pat.p, pat.p, pat.l) == 0)
            return true;
        current = current->next;
    }
    return false;
}

void freePatTable() {
    for (int i = 0; i < 5; i++) {
        HNode *current = patTable[i];
        while (current != NULL) {
            HNode *next = current->next;
            free(current);
            current = next;
        }
    }
}

void read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    char line[4096];

    // patterns
    fgets(line, sizeof(line), file);
    char *token = strtok(line, ", ");
    while(token != NULL) {
        Pat newPat = {"        ",0};
        newPat.l = strlen(token);
        if (token[newPat.l - 1] == '\n')
            newPat.l--;
        strncpy(newPat.p, token, newPat.l);
        patInsert(newPat);
        token = strtok(NULL, ", ");
    }
    
    // designs
    fgets(line,sizeof(line), file);

    while(fgets(line, sizeof(line), file)) {
        design_count++;
    }
    rewind(file);
    fgets(line,sizeof(line), file);
    fgets(line,sizeof(line), file);
    designs = (Des*)malloc(design_count * sizeof(Des));
    int i = 0;
    while(fgets(line, sizeof(line), file)) {
        Des des = {"", 0};
        des.l = strlen(line) - 1;
        strncpy(des.d, line, des.l);
        designs[i++] = des;
    }

    fclose(file);
}

bool build_design(char *design, int len) {
    if (len <= 0) return true;
    if (len > 64) return false;

    Pat pat = {{0},0};
    pat.l = len > 8 ? 8 : len;
    strncpy(pat.p, design, pat.l);

    if (len <= 8 && isInPatTable(pat))
        return true;


    for (int n = 7; n > 0; n--) {
        pat.l = n;
        if (isInPatTable(pat)) {
            if (build_design(design + n, len - n))
                return true;
        } 
    }

    return false;
}

int part_1() {
    int count = 0;
    for (int i = 0; i < design_count; i++) {
        if (build_design(designs[i].d, designs[i].l))
            count++;
    }

    return count;
}

typedef struct {
    char design[64];
    long count;
} DCount;

typedef struct DNode {
    DCount dc;
    struct DNode *next;
} DNode;

#define D_HASH_SIZE 997
DNode *dHashTable[D_HASH_SIZE] = {NULL};

int djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % D_HASH_SIZE;
}

void dHashInsert(DCount dc) {
    int index = djb2(dc.design);
    DNode *newNode = (DNode*)malloc(sizeof(DNode));
    newNode->dc = dc;
    newNode->next = dHashTable[index];
    dHashTable[index] = newNode;
}

long dHashGet(char *design) {
    int index = djb2(design);
    DNode *current = dHashTable[index];
    while (current != NULL) {
        if (strcmp(design, current->dc.design) == 0) {
            return current->dc.count;
        }
        current = current->next;
    }
    return -1;
}

void freeDHashTable() {
    for (int i = 0; i < D_HASH_SIZE; i++) {
        DNode *current = dHashTable[i];
        while (current != NULL) {
            DNode *next = current->next;
            free(current);
            current = next;
        }
    }
}

long count_design(char *design, int len) {
    if (len <= 0) return true;
    if (len > 64) return false;

    long memCount = dHashGet(design);
    if (memCount > -1) {
        return memCount;
    }

    Pat pat = {{0},0};
    pat.l = len > 8 ? 8 : len;
    strncpy(pat.p, design, pat.l);
    long count = 0;

    int sn = len > 8 ? 8 : len; 
    for (int n = sn; n > 0; n--) {
        pat.l = n;
        if (isInPatTable(pat)) {
            count += count_design(design + n, len - n);
        } 
    }

    DCount dc = {{0}, 0};
    strncpy(dc.design, design, len);
    dc.count = count;
    dHashInsert(dc);

    return count;
}


long part_2() {
    long sum = 0;
    for (int i = 0; i < design_count; i++) {
        long count = count_design(designs[i].d, designs[i].l);
        sum += count;
    }

    freeDHashTable();

    return sum;
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    read_file(argv[1]);

    int result1 = part_1();
    printf("part 1: %d\n", result1);

    long result2 = part_2();
    printf("part 2: %ld\n", result2);

    freePatTable();
    free(designs);
}