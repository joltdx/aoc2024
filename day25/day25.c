#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int **keys;
int key_count;
int **locks;
int lock_count;

void get_file_data(const char *filename) { 
    FILE *file = fopen(filename, "ra");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    char line[32];
    while(fgets(line, sizeof(line), file)) {
        if (line[0] == '#')
            lock_count++;
        else
            key_count++;

        for (int i = 0; i < 7; i++)
            fgets(line, sizeof(line), file);
    }
    rewind(file);

    keys = (int**)malloc((key_count + 1)*sizeof(int*));
    locks = (int**)malloc((lock_count + 1)*sizeof(int*));

    for (int i = 0; i < key_count; i++) {
        (keys[i] = (int*)malloc(5*sizeof(int)));
        memset(keys[i], 0, 5);
    }
    for (int i = 0; i < lock_count; i++) {
        (locks[i] = (int*)malloc(5*sizeof(int)));
        memset(locks[i], 0, 5);
    }
    
    int lock_i = 0;
    int key_i = 0;
    while(fgets(line, sizeof(line), file)) {
        if (line[0] == '#') {
            for (int i = 0; i < 6; i++) {
                fgets(line, sizeof(line), file);
                for (int j = 0; j < 5; j++) {
                    locks[lock_i][j] += line[j] == '#' ? 1 : 0;
                }
            }
            lock_i++;  
        }
        else {
            for (int i = 0; i < 5; i++) {
                fgets(line, sizeof(line), file);
                for (int j = 0; j < 5; j++) {
                    keys[key_i][j] += line[j] == '#' ? 1 : 0;
                }
            }
            key_i++;
            fgets(line, sizeof(line), file);
        }
        fgets(line, sizeof(line), file);
    }

    fclose(file);
}

void free_stuff() {
    for (int i = 0; i < key_count; i++)
        free(keys[i]);
    free(keys);

    for (int i = 0; i < lock_count; i++)
        free(locks[i]);
    free(locks);
}

int part_1() {
    int count = 0;

    for (int l = 0; l < lock_count; l++) {
        for(int k = 0; k < key_count; k++) {
            if (keys[k][0] + locks[l][0] <= 5 &&
                keys[k][1] + locks[l][1] <= 5 &&
                keys[k][2] + locks[l][2] <= 5 &&
                keys[k][3] + locks[l][3] <= 5 &&
                keys[k][4] + locks[l][4] <= 5 )
            {
                count++;
            }
        }
    }

    return count;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    get_file_data(argv[1]); 

    int result1 = part_1( );
    printf("part 1: %d\n", result1);
    printf("part 2: n/a\n");

    free_stuff();
    return 0;
}