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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    get_file_data(argv[1]);

    long long result1 = part_1( );
    printf("part 1: %lld\n", result1);

    return 0;
}