#include <stdio.h>
#include <stdlib.h>

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

int groups[26*26][100] = {-1};
int grp_size[26*26] = {0};

void build_group(int start, int add) {
    printf("%2s, add %2s\n", get_comp_id(start), get_comp_id(add));
    int *adding = comp[add].links;

    for (int i = -1; i < comp[add].l_count; i++) {
        int add_this = i == -1 ? add : adding[i];
        int breaking = 0;
        for (int k = 0; k < grp_size[start]; k++) {
            if (groups[start][k] == add_this) {
                printf("foo\n");
                breaking = 1;
                break;
            }
        }
        if (!breaking) {
            groups[start][grp_size[start]++] = add_this;
            build_group(start, add_this);
        }

    }
    
}

int part_2() {
    int count = 0;

    for (int i = 0; i < 26*26; i++) {
        for (int j = 0; j < comp[i].l_count; j++) {
            build_group(i, comp[i].links[j]);
        }
    }

    for (int i = 0; i < grp_size[get_comp_index("ka")]; i ++) {
        printf("%2s ", get_comp_id(groups[get_comp_index("ka")][i]));
    }
    printf("\n");

    return count;

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
    
    int result2 = part_2();
    printf("part 2: %d\n", result2);

}