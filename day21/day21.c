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


typedef struct {
    int x;
    int y;
} XY;

const XY KPD1[11] = {{1,3},{0,2},{1,2},{2,2},{0,1},{1,1},{2,1},{0,0},{1,0},{2,0},{2,3}};
XY kpd1_pos = {2,3};

char rpd_out[25][5] = {{0}};
char rpd_prev[25] = {' '};
char rpd_pos[25] = {'A'};

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
            strcpy(rpd_out[n], "v<A");
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
            strcpy(rpd_out[n], ">^A");
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


void generate_keypad_2(char *seq, char keys[2][32]) {

    memset(keys, 0, 2 * 32 * sizeof(char));

    int pos = 0;
    char c;
    int i = 0;
    while (c = seq[i++]) {
        //printf("%s", keypad_2(c));
        //strcpy(keys[0]+pos, keypad_2(c));
        strcpy(keys[0]+pos, robot_pad(c,2));
        pos = strlen(keys[0]);
    }
}

void generate_keypad_4(char *seq, char keys[2][32]) {

    memset(keys, 0, 2 * 32 * sizeof(char));

    int pos = 0;
    char c;
    int i = 0;
    while (c = seq[i++]) {
        //printf("%s", keypad_2(c));
        //strcpy(keys[0]+pos, keypad_3(c));
        //char *blah = robot_pad(c,4);
        strcpy(keys[0]+pos, robot_pad(c,3));
        pos = strlen(keys[0]);
    }
}

void init_stuff() {
    kpd1_pos = (XY){2,3};
    for(int i = 0; i < 25; i++) {
        rpd_prev[i] = ' ';
        rpd_pos[i] = 'A';
    }
}

int part_1( ) {
    int sum = 0;
    
    for (int i = 0; i < 5; i++) {
        char *pin = codes[i];
        init_stuff();

        char keys[2][16] = {0};
        char keys_2[2][32] = {0}; 
        char keys_3[2][32] = {0};
        int p = 0;
        int c;
        int f = 10;
        int length = 0;
        while (c = pin[p++]) {
            int len1 = INT_MAX;
            int len2 = INT_MAX;
            int t = c == 'A' ? 10 : c - '0';
            generate_keypad_1(f, t, keys);
            if (keys[0][0] != '\0') {
                generate_keypad_2(keys[0], keys_2);
                generate_keypad_4(keys_2[0], keys_3);
                //printf("%2d -> %2d: %s - %s - %s\n", f, t, keys[0], keys_2[0], keys_3[0]);
                len1 = strlen(keys_3[0]);
            }
            if (keys[1][0] != '\0') {
                generate_keypad_2(keys[1], keys_2);
                generate_keypad_4(keys_2[0], keys_3);
                //printf("%2d -> %2d: %s - %s - %s\n", f, t, keys[1], keys_2[0], keys_3[0]);
                len2 = strlen(keys_3[0]);
            }
            f = t;
            length += len2 < len1 ? len2 : len1;
        }
        int complexity = atoi(pin);
        sum += complexity * length;
    //printf("%s: %d * %d = %d\n", pin, complexity, length, complexity * length);
        //char keys_2[2][16];
        //generate_keypad_2("^^A", keys_2);
        //printf("%s\n", keys_2[0]);
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

    return 0;
}