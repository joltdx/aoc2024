#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int *instr = {NULL};
int instr_count = 0;

void initialize_debugger(const char *filename, long *regA, long *regB, long *regC, int **instr, int *instr_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    char instructions[128];
    fscanf(file, "Register A: %ld\n", regA);
    fscanf(file, "Register B: %ld\n", regB);
    fscanf(file, "Register C: %ld\n", regC);
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

long get_combo_operand(const int operand, const long regA, const long regB, const long regC) {
    long combo = 0;
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

char* part_1(const long initA, const long initB, const long initC) {
    long regA = initA;
    long regB = initB;
    long regC = initC;

    char *output = (char*)malloc(64*sizeof(char));
    memset(output, 0, 64);
    int output_pos = 0;

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
                output[output_pos++] = ',';
            output[output_pos++] = '0' + get_combo_operand(operand, regA, regB, regC) % 8;
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
    return output;
}

long part_2(long regA, int n) {
    if (n == -1)
        return regA >> 3;

    char *next_exp_output = (char*)malloc(64*sizeof(char));
    memset(next_exp_output, 0, 64);
    int output_pos = 0;

    for (int i = n; i < instr_count; i++) {
        next_exp_output[output_pos++] = '0' + instr[i];
        if ( i != instr_count - 1)
            next_exp_output[output_pos++] = ',';
    }

    for (int i = 0; i < 16; i++) {
        long tryRegA = regA + i;
        long regB = 0;
        long regC = 0;
        char *output = part_1(tryRegA, regB, regC);

        if (strcmp(output, next_exp_output) == 0) {
            // yes
            long nextTryRegA = ( regA + i ) << 3;
            long nextRegA = part_2(nextTryRegA, n - 1);
            if (nextRegA >= 0) {
                free(next_exp_output);
                return nextRegA;
            }
        }
    }

    free(next_exp_output);

    return -1;
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    long regA = 0;
    long regB = 0;
    long regC = 0;

    initialize_debugger(argv[1], &regA, &regB, &regC, &instr, &instr_count);

    char *result1 = part_1(regA, regB, regC);
    printf("part 1: %s\n", result1);

    free(result1);

    long result2 = part_2(0, instr_count - 1);
    printf("part 2: %lld\n", result2);

    return 0;
}