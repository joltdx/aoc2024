#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

char codes[5][5]= {0};

void read_codes(const char *filename) {
    FILE *file = fopen(filename, "ra");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    for (int i = 0; i < 5; i++) {
        fscanf(file, "%s", &codes[i]);
    }

    fclose(file);
}


typedef struct Node {
    char keys[32];
    int num;
    long count;
    struct Node *next;
} Node;

#define HASH_SIZE 997
Node *hashTable[HASH_SIZE] = {NULL};

int hash(const char *str, int num) {
    // djb2 of string + number
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    hash = ((hash << 5) + hash) + num;
    return hash % HASH_SIZE;
}

int hashInsertCount = 0;

void hashInsert(char *keys, int num, long count) {
    int index = hash(keys, num);
    Node *newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->keys, keys);
    newNode->next = hashTable[index];
    newNode->num = num;
    newNode->count = count;
    hashTable[index] = newNode;
}

Node *hashGet(char *keys, int num) {
    int index = hash(keys, num);
    Node *current = hashTable[index];
    while(current != NULL) {
        if (current->num == num && strcmp(current->keys, keys) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void freeHashTable() {
    for (int i = 0; i < HASH_SIZE; i++) {
        Node *current = hashTable[i];
        while (current != NULL) {
            Node *next = current->next;
            free(current);
            current = next;
        }
    }
}

typedef struct {
    int x;
    int y;
} XY;

const XY KPD1[11] = {{1,3},{0,2},{1,2},{2,2},{0,1},{1,1},{2,1},{0,0},{1,0},{2,0},{2,3}};
XY kpd1_pos = {2,3};

char rpd_out[26][5] = {{0}};
char rpd_prev[26] = {' '};
char rpd_pos[26] = {'A'};

char *robot_pad(char c, int n) {
    if (c == rpd_prev[n]) {
        strcpy(rpd_out[n], "A");
    }
    else if (c == '^') {
        if (rpd_pos[n] == 'v')
            strcpy(rpd_out[n], "^A");
        else if (rpd_pos[n] == '<')
            strcpy(rpd_out[n], ">^A");     
        else if (rpd_pos[n] == '>')
            strcpy(rpd_out[n], "<^A");
        else if (rpd_pos[n] == 'A')
            strcpy(rpd_out[n], "<A");
    }
    else if (c == 'v') {
        if (rpd_pos[n] == '^')
            strcpy(rpd_out[n], "vA");
        else if (rpd_pos[n] == '<')
            strcpy(rpd_out[n], ">A");
        else if (rpd_pos[n] == '>')
            strcpy(rpd_out[n], "<A");
        else if (rpd_pos[n] == 'A')
            strcpy(rpd_out[n], "<vA");     
    }
    else if (c == '<') {
        if (rpd_pos[n] == '^')
            strcpy(rpd_out[n], "v<A");
        else if (rpd_pos[n] == 'v')
            strcpy(rpd_out[n], "<A");
        else if (rpd_pos[n] == '>')
            strcpy(rpd_out[n], "<<A");
        else if (rpd_pos[n] == 'A')
            strcpy(rpd_out[n], "v<<A");
    }
    else if (c == '>') {
        if (rpd_pos[n] == '^')
            strcpy(rpd_out[n], "v>A");
        else if (rpd_pos[n] == 'v')
            strcpy(rpd_out[n], ">A");
        else if (rpd_pos[n] == '<')
            strcpy(rpd_out[n], ">>A");
        else if (rpd_pos[n] == 'A')
            strcpy(rpd_out[n], "vA");
    }
    else if (c == 'A') {
        if (rpd_pos[n] == '^')
            strcpy(rpd_out[n], ">A");
        else if (rpd_pos[n] == 'v')
            strcpy(rpd_out[n], "^>A"); 
        else if (rpd_pos[n] == '<')
            strcpy(rpd_out[n], ">>^A");
        else if (rpd_pos[n] == '>')
            strcpy(rpd_out[n], "^A");
    }

    rpd_pos[n] = c;
    rpd_prev[n] = c;

    return rpd_out[n];
}


void generate_keypad_1(int from, int to, char keys[2][16]) {
    XY f = KPD1[from];
    XY t = KPD1[to];

    if (from == to)
        return;

    memset(keys, 0, 2 * 16 * sizeof(char));

    int opt = 0;
    int pos = 0;
    char c;
    if (f.x == t.x) {
        int steps = t.y - f.y;
        c = steps > 0 ? 'v' : '^';
        for (int s = 0; s < abs(steps); s++) {
            keys[opt][pos++] = c;
        }
        keys[opt][pos++] = 'A';
    } else if (f.y == t.y) {
        int steps = t.x - f.x;
        c = steps > 0 ? '>' : '<';
        for (int s = 0; s < abs(steps); s++) {
            keys[opt][pos++] = c;
        }
        keys[opt][pos++] = 'A';
    } else {
        if (f.x == 0 && t.y == 3) {
            // first >, then v;
            int steps = t.x - f.x;
            c = steps > 0 ? '>' : '<';
            for (int s = 0; s < abs(steps); s++) {
                keys[opt][pos++] = c;
            }
            steps = t.y - f.y;
            c = steps > 0 ? 'v' : '^';
            for (int s = 0; s < abs(steps); s++) {
                keys[opt][pos++] = c;
            }
            keys[opt][pos++] = 'A';
        } else if (f.y == 3 && t.x == 0) {
            // first ^, then <
            int steps = t.y - f.y;
            c = steps > 0 ? 'v' : '^';
            for (int s = 0; s < abs(steps); s++) {
                keys[opt][pos++] = c;
            }
            steps = t.x - f.x;
            c = steps > 0 ? '>' : '<';
            for (int s = 0; s < abs(steps); s++) {
                keys[opt][pos++] = c;
            }
            keys[opt][pos++] = 'A';
        } else {
            // option 1, first <>, then ^v
            int steps = t.x - f.x;
            c = steps > 0 ? '>' : '<';
            for (int s = 0; s < abs(steps); s++) {
                keys[opt][pos++] = c;
            }
            steps = t.y - f.y;
            c = steps > 0 ? 'v' : '^';
            for (int s = 0; s < abs(steps); s++) {
                keys[opt][pos++] = c;
            }
            keys[opt][pos++] = 'A';

            // option 2, first ^v, then <>
            opt++;
            pos = 0;
            steps = t.y - f.y;
            c = steps > 0 ? 'v' : '^';
            for (int s = 0; s < abs(steps); s++) {
                keys[opt][pos++] = c;
            }
            steps = t.x - f.x;
            c = steps > 0 ? '>' : '<';
            for (int s = 0; s < abs(steps); s++) {
                keys[opt][pos++] = c;
            }
            keys[opt][pos++] = 'A';
        }
    }
}

void generate_robot_pad(char *seq, char *keys, int n) {
    memset(keys, 0, 64 * sizeof(char));
    int pos = 0;
    char c;
    int i = 0;
    while (c = seq[i++]) {
        strcpy(keys+pos, robot_pad(c,n));
        pos = strlen(keys);
    }
}

void init_stuff() {
    kpd1_pos = (XY){2,3};
    for(int i = 0; i < 26; i++) {
        rpd_prev[i] = ' ';
        rpd_pos[i] = 'A';
    }
}

long robot_pad_pressing(char *seq, int num) {
    if (num == 0) {
        return(strlen(seq));
    }

    Node *memoized = hashGet(seq, num);
    if (memoized != NULL) {
        return memoized->count;
    }

    char next_seq[64] = {0};
    char pad_seq[64] = {0};
    char go_seq[64] = {0};

    long sum = 0;
    int pos = 0;
    for (int n = 0; seq[n] != '\0'; n++) {
        if (seq[n] == 'A') {
            strncpy(go_seq, seq+pos, n+1-pos);
            go_seq[n+1-pos] = '\0';
            generate_robot_pad(go_seq, next_seq, num);
            sum += robot_pad_pressing(next_seq, num - 1);
            pos = n + 1;
        }
    }

    hashInsert(seq, num, sum);

    return sum;
}

int part_1( ) {
    int sum = 0;
    
    for (int i = 0; i < 5; i++) {
        char *pin = codes[i];
        init_stuff();

        char keys[2][16] = {0};
        char keys_2[64] = {0}; 
        char keys_3[64] = {0};
        int p = 0;
        int c;
        int f = 10;
        int length = 0;
        while (c = pin[p++]) {
            long len1 = LONG_MAX;
            long len2 = LONG_MAX;
            int t = c == 'A' ? 10 : c - '0';
            generate_keypad_1(f, t, keys);
            if (keys[0][0] != '\0') {
                len1 = robot_pad_pressing(keys[0], 2);
            }
            if (keys[1][0] != '\0') {
                len2 = robot_pad_pressing(keys[1], 2);
            }
            f = t;
            length += len2 < len1 ? len2 : len1;
        }
        int complexity = atoi(pin);
        sum += complexity * length;
    }

    return sum;
}

long part_2( ) {
    long sum = 0;
    
    for (int i = 0; i < 5; i++) {
        char *pin = codes[i];
        init_stuff();

        char keys[2][16] = {0};
        char keys_2[64] = {0}; 
        char keys_3[64] = {0};
        int p = 0;
        int c;
        int f = 10;
        long length = 0;
        while (c = pin[p++]) {
            long len1 = LONG_MAX;
            long len2 = LONG_MAX;
            int t = c == 'A' ? 10 : c - '0';
            generate_keypad_1(f, t, keys);
            if (keys[0][0] != '\0') {
                len1 = robot_pad_pressing(keys[0], 25);
            }
            if (keys[1][0] != '\0') {
                len2 = robot_pad_pressing(keys[1], 25);
            }
            f = t;
            length += len2 < len1 ? len2 : len1;
        }
        int complexity = atoi(pin);
        sum += complexity * length;
    }

    return sum;
}

int main(int argc, char **argv) {

    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Usage: %s <input> [test]\n", argv[0]);
        exit(1);
    }

    read_codes(argv[1]);


    int result1 = part_1();
    printf("part 1: %d\n", result1);
    long result2 = part_2();
    printf("part 2: %ld\n", result2);

    freeHashTable();

    return 0;
}