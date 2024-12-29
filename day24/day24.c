#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int OPER_AND = 1;
const int OPER_OR = 2;
const int OPER_XOR = 3;

typedef char Wid[4];

typedef struct {
    Wid w;
    int s;
} Wire;

typedef struct {
    Wid a;
    int o;
    Wid b;
    Wid t;
    int resolved;
} Gate;

Wire wires[1000] = {0};
int wire_count = 0;
Gate gates[1000] = {0};
int gate_count = 0;
int z_wire_count = 0;

void get_file_data(const char *filename) {
    FILE *file = fopen(filename, "ra");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    char line[32];
    while(fgets(line, sizeof(line), file)) {
        if (line[0] == '\n')
            continue;

        Wid wid;
        int state;
        char oper[4];
        if(sscanf(line, "%3s: %d", &wires[wire_count].w, &wires[wire_count].s) == 2) {
            wire_count++;
        }
        else if (sscanf(line, "%s %s %s -> %s", &gates[gate_count].a, &oper, &gates[gate_count].b, &gates[gate_count].t) == 4) {
            if (strncmp(oper, "AND", 3) == 0) {
                gates[gate_count].o = OPER_AND;
            }
            else if (strncmp(oper, "OR", 2) == 0) {
                gates[gate_count].o = OPER_OR;
            }
            else if (strncmp(oper, "XOR", 3) == 0) {
                gates[gate_count].o = OPER_XOR;
            }
            if (gates[gate_count].t[0] == 'z')
                z_wire_count++;
            gate_count++;
        }
    }
    fclose(file);
}

int get_operand(Wid wid) {
    for (int i = 0; i < wire_count; i++) {
        if (strncmp(wires[i].w, wid, 3) == 0) {
            return wires[i].s;
        }
    }
    return -1;
}

void add_operand(Wid wid, int state) {
    strncpy(wires[wire_count].w, wid, 3);
    wires[wire_count].w[3] = '\0';
    wires[wire_count].s = state;
    wire_count++;
}

long long part_1( ) {
    int z_resolved = 0;
    while(z_resolved != z_wire_count) {
        for (int i = 0; i < gate_count; i++) {
            if (gates[i].resolved == 1)
                continue;

            int oper_a = get_operand(gates[i].a);
            int oper_b = get_operand(gates[i].b);
            if (oper_a == -1 || oper_b == -1) {
                continue;
            }

            if (gates[i].o == OPER_AND) {
                add_operand(gates[i].t, oper_a & oper_b);
            }
            else if (gates[i].o == OPER_OR) {
                add_operand(gates[i].t, oper_a | oper_b);
            }
            else if (gates[i].o == OPER_XOR) {
                add_operand(gates[i].t, oper_a ^ oper_b);
            }
            gates[i].resolved = 1;
            if (gates[i].t[0] == 'z') {
                z_resolved++;
            }
        }
    }

    long long result = 0;
    long long z = 0;
    Wid wid = "z  ";
    while(1) {
        wid[1] = '0' + ( z / 10 );
        wid[2] = '0' + ( z % 10 );
        long long zstate = get_operand(wid);
        if (zstate == -1)
            break;
        long long zvalue = zstate << z;
        result += zvalue;
        z++;
    }
    
    return result;
}

int find_gate(Wid g, int op, Wid *output) {
    for (int i = 0; i < gate_count; i++) {
        if (gates[i].o == op && ((strncmp(gates[i].a, g, 3) == 0 ) || (strncmp(gates[i].b, g, 3)) == 0)) {
            strcpy(*output, gates[i].t);
            return i;
        }
    }
    return -1;
}

int find_output(Wid output) {
    for (int i = 0; i < gate_count; i++) {
        if (strncmp(gates[i].t, output, 3) == 0 ) {
            return i;
        }
    }
    return -1;
}


void print_gate(const int i) {
    char operation[4] = {0};
    if (gates[i].o == OPER_AND)
        strcpy(operation, "AND");
    else if (gates[i].o == OPER_OR)
        strcpy(operation, "OR ");
    else
        strcpy(operation, "XOR");

    printf("%s %s %s -> %s\n", gates[i].a, operation, gates[i].b, gates[i].t);
}

void get_numbered_ids(const int i, Wid *x1, Wid *x2) {
    (*x1)[0] = 'x'; 
    (*x1)[1] = '0' + ( i / 10 );
    (*x1)[2] = '0' + ( i % 10 );
    (*x1)[3] = '\0';
    (*x2)[0] = 'x'; 
    (*x2)[1] = '0' + ( ( i + 1 ) / 10 );
    (*x2)[2] = '0' + ( ( i + 1 ) % 10 );
    (*x2)[3] = '\0';
}

Wid swapped[8] = {0};
int swapped_count = 0;

void add_swapped(Wid wid) {
    int i;
    for (i = 0; i < 8 && swapped[i][0] != 0; i++) {
        int cmp = strcmp(swapped[i], wid);
        if (cmp == 0)
            return;     // duplicate
        if (cmp > 0)
            break;      // insert here
    }

    if (i >= 8)
        return;

    for (int j = 7; j > i; j--)
        strcpy(swapped[j], swapped[j - 1]);

    strcpy(swapped[i], wid);
}

void part_2() {
    // This is how it's supposed to be:
    // x00 XOR y00 -> z00

    // x00 AND y00 -> c1    
    // x01 XOR y01 -> t1    
    // t1  XOR c1  -> z01   
 
    // x01 AND y01 -> ca    
    // c1  AND t1  -> cb    
    // ca  OR  cb  -> c1    
    // x02 XOR y02 -> t1    
    // t1  XOR c1  -> z02   

    // and we can increase expected x, y and z, and loop from here...
    // x02 AND y02 -> ca    
    // c1  AND t1  -> cb    
    // ca  OR  cb  -> c1    
    // x03 XOR y03 -> t1   
    // t1  XOR c1  -> z03   

    // etc... Let's make sure it's consistent...
    Wid x1, x2, c1, ca, cb, t1, z2, dummy;

    // Manually check that we're ok before the loop starts,
    // then we need the c1 and can enter looping
    int i = find_gate("x00", OPER_AND, &c1);

    for (int i = 1; i < z_wire_count - 2; i++) {
        get_numbered_ids(i, &x1, &x2); 
        int id_ca = find_gate(x1, OPER_AND, &ca);
        if (ca[0] == 'z')
            add_swapped(ca);

        int id_cb = find_gate(c1, OPER_AND, &cb);
        if (id_cb == -1)
            add_swapped(c1);
        else if (cb[0] == 'z')
            add_swapped(cb);

        int id_c1 = find_gate(ca, OPER_OR, &c1);
        if (id_c1 == -1)
            add_swapped(ca);
        else if (c1[0] == 'z')
            add_swapped(c1);

        int id_t1 = find_gate(x2, OPER_XOR, &t1);
        if (t1[0] == 'z')
            add_swapped(t1);

        int id_z2 = find_gate(t1, OPER_XOR, &z2);
        if (id_z2 == -1)
            add_swapped(t1);
        else if (z2[0] != 'z')
            add_swapped(z2);
    }

    printf("part 2: ");
    for (int i = 0; i < 8 && swapped[i][0] != 0; i++) {
        if (i > 0)
            printf(",");
        printf("%s", swapped[i]);
    }
    printf("\n");
         
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    get_file_data(argv[1]);

    long long result1 = part_1( );
    printf("part 1: %lld\n", result1);

    part_2();

    return 0;
}