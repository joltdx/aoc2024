#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Stone {
    long mark;
    struct Stone *next;
} Stone;

Stone *create_stone(long mark) {
    Stone *newStone = (Stone *)malloc(sizeof(Stone));
    newStone->mark = mark;
    newStone->next = NULL;
    return newStone;
}

int count_digits(long num) {
    if (num == 0) return 1;
    int count = 0;
    while (num != 0) {
        num /= 10;
        count++;
    }
    return count;
}

void split_stone(Stone *stone, int digits) {
    int halfDigits = digits / 2;
    int divisor = (int) pow(10, halfDigits);
    long left = stone->mark / divisor;
    long right = stone->mark % divisor;
    Stone *newStone = create_stone(right);
    newStone->next = stone->next;
    stone->next = newStone;
    stone->mark = left;
}

Stone *copy_stones(Stone *source) {
    Stone *dummyStone = create_stone(0);
    Stone *stone = dummyStone;
    while (source != NULL) {
        stone->next = create_stone(source->mark);
        stone = stone->next;
        source = source->next; 
    }
    stone = dummyStone->next;
    free(dummyStone);
    return stone;
}

Stone *read_stones_from_file(const char *filename) {
    FILE *file;

    file = fopen(filename, "ra");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    long mark = 0;

    Stone *dummyStone = create_stone(0);
    Stone *stone = dummyStone;
    while (fscanf(file, "%ld", &mark) == 1) {
        stone->next = create_stone(mark);
        stone = stone->next;
    }

    stone = dummyStone->next;
    free(dummyStone);

    fclose(file);

    return stone;
}

void free_stones(Stone *stone) {
    while(stone != NULL) {
        Stone *next = stone->next;
        free(stone);
        stone = next;
    }
}

void blink_at_stones(Stone *stone) {
    while (stone != NULL) {
        if (stone->mark == 0) {
            stone->mark = 1;
            stone = stone->next;
            continue;
        }
        int digits = count_digits(stone->mark);
        if (digits % 2 == 0) {
            split_stone(stone, digits);
            stone = stone->next->next;
            continue;
        }
        stone->mark = stone->mark * 2024;
        stone = stone->next;
    }
}

int part_1(Stone *stone, const int blinks) {
    for (int i = 0; i < blinks; i++)
        blink_at_stones(stone);

    int count = 0;
    while(stone != NULL) {
        count++;
        stone = stone->next;
    }

    return count;
}


// Right, for part 2 just doing the thing is not feasible.
// Let's memoize this sh*t with some kind of hash situation
typedef struct {
    long mark;
    long count[128];
} Count;

typedef struct HNode {
    Count count;
    struct HNode *next;
} HNode;

#define HASH_SIZE 1000
HNode *hashTable[HASH_SIZE] = {NULL};

int hash(long mark) {
    return mark % HASH_SIZE;
}

HNode *hash_insert(long mark) {
    int index = hash(mark);
    HNode *newNode = (HNode *)malloc(sizeof(HNode));
    newNode->count.mark = mark;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
    return newNode;
}

HNode *hash_find(long mark) {
    int index = hash(mark);
    HNode *current = hashTable[index];
    while (current != NULL) {
        if (current->count.mark == mark)
            return current;
        current = current->next;
    }
    return NULL;
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

typedef struct {
    long l;
    long r;
} Split;

Split blink_at_mark(long mark) {
    Split split = {-1, -1};

    if (mark == 0) {
        split.l = 1;
        return split;
    }

    int digits = count_digits(mark);
    if (digits % 2 == 0) {
        int halfDigits = digits / 2;
        long divisor = (int)pow(10, halfDigits);
        split.l = mark / divisor;
        split.r = mark % divisor;
        return split;
    }

    split.l = mark * 2024;
    return split;
}

long blink(long mark, int times) {
    if (times == 0)
        return 1;

    HNode *node = hash_find(mark);
    if (node == NULL) {
        node = hash_insert(mark);
    }

    if (node->count.count[times] != 0)
        return node->count.count[times];
    
    Split split = blink_at_mark(mark);
    if (split.l >= 0)
        node->count.count[times] += blink(split.l, times - 1);

    if (split.r >= 0)
        node->count.count[times] += blink(split.r, times - 1); 

    return node->count.count[times];
}


long part_2(Stone *stone, const int blinks) {

    long sum = 0;

    while (stone != NULL) {
        long count = blink(stone->mark, blinks);
        sum += count;
        stone = stone->next;
    }

    return sum;
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    Stone *stones = read_stones_from_file(argv[1]);
    Stone *stones_2 = copy_stones(stones);

    int result_1 = part_1(stones, 25);
    printf("part 1: %d\n", result_1);
    long result_2 = part_2(stones_2, 75);
    printf("part 2: %ld\n", result_2);

    free_stones(stones);
    free_stones(stones_2);

    freeHashTable();
}