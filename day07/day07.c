#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long val;
    long num[16];    // Let's keep it simple. Max width in input is 12...
} Calibration;

long mul_plus(long val, long sum, long *num) {
    if (num[0] == 0) return sum;

    if (sum > val) return sum;

    long next = mul_plus(val, sum * num[0], num+1);
    if (next != val) {
        next = mul_plus(val, sum + num[0], num+1);
    }
    return next;
}

long part_1(Calibration *cal) {
    long sum = 0;

    for (int c = 0; cal[c].val > 0; c++) {
        long cal_val = mul_plus(cal[c].val, cal[c].num[0], cal[c].num+1);
        if (cal_val == cal[c].val) sum += cal_val;
    }

    return sum;
}

long concatenate(long left, long right) {
    int tens = 1;
    while(tens <= right) tens *= 10;
    return left * tens + right;
}

long mul_plus_concat(long val, long sum, long *num) {
    if (num[0] == 0) return sum;

    if (sum > val) return sum;

    long next = mul_plus_concat(val, sum * num[0], num+1);
    if (next != val) {
        next = mul_plus_concat(val, sum + num[0], num+1);
    }
    if (next != val) {
        next = mul_plus_concat(val, concatenate(sum, num[0]), num+1);
    }
    return next;
}

long part_2(Calibration *cal) {
    long sum = 0;

    for (int c = 0; cal[c].val > 0; c++) {
        long cal_val = mul_plus_concat(cal[c].val, cal[c].num[0], cal[c].num+1);
        if (cal_val == cal[c].val) sum += cal_val;
    }

    return sum;
}

int main(int argc, char **argv) {
    FILE *file;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", argv[1]);
        exit(1);
    }

    Calibration cal[1000]; // Let's keep it simple. 850 lines in input...
    char input[64];
    int cal_n = 0;
    while (fgets(input, sizeof(input), file))
    {
        char *token = strtok(input, ":");
        while (token != NULL)
        {
            cal[cal_n].val = atol(token);
            token = strtok(NULL, " ");
            int num_n = 0;
            while (token != NULL)
            {
                cal[cal_n].num[num_n] = atoll(token);
                token = strtok(NULL, " ");
                num_n++;
            }
            cal[cal_n].num[num_n] = 0;
        }
        cal_n++;
    }
    cal[cal_n].val = 0;

    fclose(file);

    long result1 = part_1(cal);
    printf("part 1: %ld\n", result1);

    long result2 = part_2(cal);
    printf("part 2: %ld\n", result2);

    return 0;
}