#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void initialize_debugger(const char *filename, int *regA, int *regB, int *regC, int **instr, int *instr_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    char instructions[128];
    fscanf(file, "Register A: %d\n", regA);
    fscanf(file, "Register B: %d\n", regB);
    fscanf(file, "Register C: %d\n", regC);
    fscanf(file, "\n");
    fscanf(file, "Program: %s\n", &instructions);

    *instr_count = ( strlen(instructions) + 1 ) / 2;

    *instr = (int *)malloc(*instr_count * sizeof(int));
    char *token = strtok(instructions, ",");
    int i = 0;
    while (token != NULL) {
        (*instr)[i++] = atoi(token);
        token = strtok(NULL, ",");
    }

    fclose(file);
}

int get_combo_operand(const long operand, const long regA, const long regB, const long regC) {
    int combo = 0;
    if (operand <= 3)
        combo = operand;
    else if (operand == 4)
        combo = regA;
    else if (operand == 5)
        combo = regB;
    else if (operand == 6)
        combo = regC;
    return combo;
}

void part_1(const int initA, const int initB, const int initC, const int instr[], const int instr_count) {
    int regA = initA;
    int regB = initB;
    int regC = initC;

    printf("result 1: ");
    int outed = 0;
    int i = 0;
    while (i >= 0 && i < instr_count) {
        int opcode = instr[i];
        int operand = instr[i+1];
        switch (opcode) {
        case 0:     // adv
            regA = regA / pow(2, get_combo_operand(operand, regA, regB, regC));
            break;
        case 1:     //bxl
            regB = regB ^ operand;
            break;
        case 2:     //bst
            regB = get_combo_operand(operand, regA, regB, regC) % 8;
            break;
        case 3:     //jnz
            if (regA != 0) {
                i = operand;
                continue;
            }
            break;
        case 4:     //bxc
            regB = regB ^ regC;
            break;
        case 5:     //out
            if (outed)
                printf(",");
            printf("%d", get_combo_operand(operand, regA, regB, regC) % 8);
            outed = 1;
            break;
        case 6:     //bdv
            regB = regA / pow(2, get_combo_operand(operand, regA, regB, regC));
            break;
        case 7:     //cdv
            regC = regA / pow(2, get_combo_operand(operand, regA, regB, regC));
            break;
        }
        i += 2;
    }
    printf("\n");
}

void part_2(const int instr[], const int instr_count) {
    long regA = 0;
    long regB = 0;
    long regC = 0;

    int output[16];
    long prev_a = 0;
    int wanted_out_n = 0;
    
    long a = 0;
    while (1) {
        regA = a;
        
        int break_out = 0;
        int out_n = 0;
        int i = 0;
        while (i >= 0 && i < instr_count && out_n < instr_count && !break_out) {
            int opcode = instr[i];
            int operand = instr[i+1];
            switch (opcode) {
            case 0:     // adv
                regA = regA / pow(2, get_combo_operand(operand, regA, regB, regC));
                break;
            case 1:     //bxl
                regB = regB ^ operand;
                break;
            case 2:     //bst
                regB = get_combo_operand(operand, regA, regB, regC) % 8;
                break;
            case 3:     //jnz
                if (regA != 0) {
                    i = operand;
                    continue;
                }
                break;
            case 4:     //bxc
                regB = regB ^ regC;
                break;
            case 5:     //out
                int o = get_combo_operand(operand, regA, regB, regC) % 8;
                if (o != instr[out_n]) {
                    break_out = 1;
                    continue;
                }
                output[out_n] = o;
                out_n++;
    
                break;
            case 6:     //bdv
                regB = regA / pow(2, get_combo_operand(operand, regA, regB, regC));
                break;
            case 7:     //cdv
                regC = regA / pow(2, get_combo_operand(operand, regA, regB, regC));
                break;
            }
            i += 2;
        }
        if (out_n > wanted_out_n) {
            printf("%ld: (%ld)", a, a - prev_a);
            for (int on = 0; on < out_n; on++) {
                printf("%d ", output[on]);
            }
            printf("\n");
            prev_a = a;
            wanted_out_n++;
        }
        if (out_n == instr_count) {
            printf("part 2: %ld\n", a);
            break;
        }
        //if (a % 100000000 == 0) {
        //    printf("%ld\n", a);
        //}
        a += 1;
    }
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    int regA = 0;
    int regB = 0;
    int regC = 0;
    int *instr = NULL;
    int instr_count = 0;
    initialize_debugger(argv[1], &regA, &regB, &regC, &instr, &instr_count);

    part_1(regA, regB, regC, instr, instr_count);

    //lol no   part_2(instr, instr_count);

    return 0;
}