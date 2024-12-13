#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int x;
    int y;
} XY;

typedef struct {
    XY a;
    XY b;
    XY p;
} Machine;

Machine **read_machines(const char *filename, int *num_machines) {
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    // Find numer of lines in file
    int num_lines = 0;
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n')
            num_lines++;
    }
    if (ch != '\n')
        num_lines++;

    // Add 1 to be safe in the division
    *num_machines = (num_lines + 1 ) / 4;
    
    // Allocate memory
    Machine **machines = malloc(*num_machines * sizeof(Machine *));
    for (int m = 0; m < *num_machines; m++) {
        machines[m] = malloc(sizeof(Machine));
    }

    // Get the machines
    rewind(file);
    for (int m = 0; m < *num_machines; m++) {
        fscanf(file, "Button A: X+%d, Y+%d\n", &machines[m]->a.x, &machines[m]->a.y);
        fscanf(file, "Button B: X+%d, Y+%d\n", &machines[m]->b.x, &machines[m]->b.y);
        fscanf(file, "Prize: X=%d, Y=%d\n\n", &machines[m]->p.x, &machines[m]->p.y);
    }

    fclose(file);

    return machines;
}

void free_machines(Machine **machines, const int num_machines) {
    for (int m = 0; m < num_machines; m++) {
        free(machines[m]);
    }
    free(machines);
}

long part_1(Machine **machines, const int num_machines) {

    // Right, these are two linear equations
    // a * xb + b * xb = xp
    // a * ya + b * yb = yp
    // Let's math it:
    // b = ( xp * ya - xa * yp ) / ( xb * ya + xa * yb )
    // a = ( xp - b * xp ) / xa

    long cost = 0;

    for (int i = 0; i < num_machines; i++) {
        Machine m = *machines[i];

        // mathing:
        long b = ( m.p.x * m.a.y - m.p.y * m.a.x ) / ( m.a.y * m.b.x - m.b.y * m.a.x );
        long a = ( m.p.x - m.b.x * b ) / m.a.x;

        // verifying whole number solution
        if (m.a.x * a + m.b.x * b == m.p.x && m.a.y * a + m.b.y * b == m.p.y)
            cost += 3 * a + b;

    }

    return cost;
}

long part_2(Machine **machines, const int num_machines) {

    long cost = 0;

    long very_very_much = 10000000000000;

    for (int i = 0; i < num_machines; i++) {
        Machine m = *machines[i];

        long px = m.p.x + very_very_much;
        long py = m.p.y + very_very_much;

        // mathing:
        long b = ( px * m.a.y - py * m.a.x ) / ( m.a.y * m.b.x - m.b.y * m.a.x );
        long a = ( px - m.b.x * b ) / m.a.x;

        // verifying whole number solution
        if (m.a.x * a + m.b.x * b == px && m.a.y * a + m.b.y * b == py)
            cost += 3 * a + b;
    }

    return cost;
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    int num_machines = 0;
    Machine **machines = read_machines(argv[1], &num_machines);

    long result1 = part_1(machines, num_machines);
    printf("part 1: %ld\n", result1);
    long result2 = part_2(machines, num_machines);
    printf("part 2: %ld\n", result2);

    free_machines(machines, num_machines);
    return 0;
}
