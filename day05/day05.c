#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_file(const char *filename, int ***rules, int *num_rul, int ***updates, int *num_upd)
{
    // Heh, figuring out that I needed the *** for parameters here, and the & in the call
    // made my brain hurt. Also, could've just set sizes statically, but wanted to figure
    // out how all this works... 
    //
    // Also, this took way longer than the puzzle itself. All good \o/

    // Open file
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    // count stuff in file
    *num_rul = 0;
    *num_upd = 0;
    int max_upd_pos = 0;
    char dummy_line[128];
    // number of page ordering rules
    while (fgets(dummy_line, sizeof(dummy_line), file)) {
        if (strcmp(dummy_line, "\n") == 0) {
            break;
        }
        *num_rul += 1;
    }
    // number of updates
    while (fgets(dummy_line, 128, file) != NULL) {
        *num_upd += 1;

        // also look for max number of positions in an update
        char *str = dummy_line;
        int upd_pos = 1;
        for (; *str; str++) upd_pos += (*str == ',');   // counting number of ','
        if (upd_pos > max_upd_pos) max_upd_pos = upd_pos;
    }

    // rewind file to read from beginning
    rewind(file);

    // allocate memory for rules
    *rules = (int**)malloc((*num_rul + 1) * sizeof(int*));
    if (rules == NULL) {
        fprintf(stderr, "Error allocating memory for rules buffer\n");
        fclose(file);
        exit(1);
    }

    for (int i=0; i<*num_rul + 1; i++) {
        (*rules)[i] = (int*)malloc(2 * sizeof(int));
        if ((*rules)[i] == NULL) {
            fprintf(stderr, "Error allocating memory for rule %d\n",i);
            for (int j=0; j < i; j++) {
                free((*rules)[j]);
            }
            free(*rules);
            fclose(file);
            exit(1);
        }
    }

    // alocate memory for updates
    *updates = (int**)malloc((*num_upd + 1) * sizeof(int*));
    if (*updates == NULL) {
        fprintf(stderr, "Error allocating memory for updates buffer\n");
        for (int j=0; j < *num_rul + 1; j++) {
            free((*rules)[j]);
        }
        free(*rules);
        fclose(file);
        exit(1);
    }

    // allocate memory for each update
    for (int i = 0; i < *num_upd + 1; i++) {
        (*updates)[i] = (int*)malloc((max_upd_pos + 1) * sizeof(int));
        if ((*updates)[i] == NULL) {
            fprintf(stderr, "Error allocating memory for update index %d\n", i);
            for (int j = 0; j < i; j++)
            {
                free((*updates)[j]);
            }
            free(*updates);
            for (int j = 0; j < *num_rul + 1; j++) {
                free((*rules)[0]);
                free((*rules)[1]);
            }
            free(*rules);
            fclose(file);
            exit(1);
        }
    }


    // read page ordering rules, example 46|53 
    int line[2];
    int rule_index = 0;
    for (int i = 0; i < *num_rul; i++) {
        if(fscanf(file, "%d|%d", &(*rules)[rule_index][0], &(*rules)[rule_index][1]) == 2) {
            rule_index++;
        }
    }

    // skip 2 line breaks ahead :D
    fgets(dummy_line, 128, file);
    fgets(dummy_line, 128, file);

    // read updates
    int update = 0;
    while (fgets(dummy_line, 128, file) != NULL) {
        char *token = strtok(dummy_line, ",");
        int index = 0;
        while (token != NULL) {
            (*updates)[update][index] = atoi(token);
            token = strtok(NULL, ",");
            index++;
        }
        update++;
    }

    fclose(file);
    return;

}

void free_data(int ***rules, int num_rul, int ***updates, int num_upd) 
{
    // return all the memory :)
    for (int i = 0; i < num_rul; i++) {
        free((*rules)[i]);
    }
    free(*rules);

    for (int i = 0; i < num_upd; i++)
    {
        free((*updates)[i]);
    }
    free(*updates);

}

int part_1(int **rules, const int num_rul, int **updates, const int num_upd)
{
    int sum = 0;

    for (int update = 0; update < num_upd; update++) {
        int l = updates[update][0];
        int r = updates[update][1];
        int ok = 1;
        int pos = 2;
        while(r != 0) {
            for (int rule = 0; rule < num_rul; rule++) {
                if (rules[rule][0] == r && rules[rule][1] == l) {
                    ok = 0;
                    break;
                }
            }
            if (ok == 0) {
                break;
            }          
            l = r;
            r = updates[update][pos];
            pos++;
        }
        if (ok == 1) {
            sum += updates[update][pos/2 - 1];
        }
    }
    
    return sum;
}

int part_2(int **rules, const int num_rul, int **updates, const int num_upd)
{
    int sum = 0;

    for (int update = 0; update < num_upd; update++) {
        int l = updates[update][0];
        int r = updates[update][1];
        int fixed = 0;
        int pos = 2;
        while(r != 0) {
            for (int rule = 0; rule < num_rul; rule++) {
                if (rules[rule][0] == r && rules[rule][1] == l) {
                    updates[update][pos-2] = r;
                    updates[update][pos-1] = l;
                    fixed = 1;
                    r = updates[update][0];
                    pos = 1;
                    break;
                }
            }
            l = r;
            r = updates[update][pos];
            pos++;
        }
        if (fixed == 1) {
            sum += updates[update][pos/2 - 1];
        }
    }
    
    return sum;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    int **rules = NULL;
    int **updates = NULL;
    int num_rul;
    int num_upd;

    read_file(argv[1], &rules, &num_rul, &updates, &num_upd);

    int sum1 = part_1(rules, num_rul, updates, num_upd);
    printf("part 1: %d\n", sum1);

    int sum2 = part_2(rules, num_rul, updates, num_upd);
    printf("part 2: %d\n", sum2);

    free_data(&rules, num_rul, &updates, num_upd);

    return 0;
}