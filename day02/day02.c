#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int safety_dance(int data[8])
{
    int prev = data[0];
    int safe = 0;

    if (data[1] == prev)
    {
        // nope
    }
    else if (data[1] < prev)
    {
        int this_is_safe = 1;
        for (int j = 1; j < 8 && data[j] > 0; j++)
        {
            int diff = prev - data[j];
            if (diff < 1 || diff > 3)
            {
                this_is_safe = 0;
                break;
            }
            prev = data[j];
        }
        safe += this_is_safe;
    }
    else
    {
        int this_is_safe = 1;
        for (int j = 1; j < 8 && data[j] > 0; j++)
        {
            int diff = data[j] - prev;
            if (diff < 1 || diff > 3)
            {
                this_is_safe = 0;
                break;
            }
            prev = data[j];
        }
        safe += this_is_safe;
    }

    return safe;
}

int part1(int data[1000][8])
{
    int safe = 0;

    for (int i = 0; i < 1000 && data[i][0] > 0; i++)
    {
        safe += safety_dance(data[i]);
    }

    return safe;
}

int part2(int data[1000][8])
{
    int safe = 0;
    int report[8] = {0};

    for (int i = 0; i < 1000 && data[i][0] > 0; i++)
    {
        int report_safe = safety_dance(data[i]);
        if (report_safe > 0)
        {
            safe += report_safe;
            continue;
        }
        for (int j = 0; j < 8; j++)
        {
            int index = 0;
            for (int k = 0; k < 8; k++)
            {
                if (k == j)
                    continue;
                report[index] = data[i][k];
                index++;
            }
            report_safe = safety_dance(report);
            if (report_safe > 0)
            {
                safe += report_safe;
                break;
            }
        }
    }
    return safe;
}

int main(int argc, char **argv)
{
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
    }

    char input[32];
    int data[1000][8] = {0};

    int report = 0;
    while (fgets(input, sizeof(input), file))
    {
        char *token = strtok(input, " ");
        int level = 0;
        while (token != NULL)
        {
            data[report][level] = atoi(token);
            token = strtok(NULL, " ");
            level++;
        }
        report++;
    }

    fclose(file);

    int result1 = part1(data);
    printf("part 1: %d\n", result1);
    int result2 = part2(data);
    printf("part 2: %d\n", result2);

    return 0;
}
