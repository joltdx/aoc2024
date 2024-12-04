#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **read_file(const char *filename, int *num_lines, int *line_length)
{
    // Open file
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

    // Read one line to determine length (in this case, all lines are equal length)
    char buffer_line_length[1024];
    int file_width = 0;
    if (fgets(buffer_line_length, 1024, file))
    {
        file_width = strlen(buffer_line_length);
        *line_length = file_width - 1; // do not include end-of-line character
    }
    else
    {
        fprintf(stderr, "Could not get line length from file\n");
        fclose(file);
        exit(1);
    }

    // Go to the end of file to determine total file size
    // Number of lines = total size / line length
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file) + 1;
    *num_lines = file_size / file_width;
    if (*num_lines < 1)
    {
        fprintf(stderr, "Could not get number of lines from file\n");
        fclose(file);
        exit(1);
    }

    // We want to start reading again at the beginning of the file
    rewind(file);

    // Allocate memory for lines buffer
    char **lines = malloc(*num_lines * sizeof(char *));
    if (lines == NULL)
    {
        fprintf(stderr, "Error allocating memory for lines buffer\n");
        fclose(file);
        exit(1);
    }

    // allocate memory for all of the lines
    for (int i = 0; i < *num_lines; i++)
    {
        lines[i] = malloc((*line_length + 1) * sizeof(char));
        if (lines[i] == NULL)
        {
            fprintf(stderr, "Error allocating memory for line index %d\n", i);
            for (int j = 0; j < i; j++)
            {
                free(lines[j]);
            }
            free(lines);
            fclose(file);
            exit(1);
        }
    }

    // read all of the lines into lines buffer
    for (int i = 0; i < *num_lines; i++)
    {
        if (fgets(lines[i], *line_length + 2, file) == NULL)
        {
            fprintf(stderr, "Error reading file line %d\n", i + 1);
            for (int j = 0; j < 1; j++)
            {
                free(lines[j]);
            }
            free(lines);
            fclose(file);
            exit(1);
        }
        int len = strlen(lines[i]);
        if (len > 0 && lines[i][len - 1] == '\n')
        {
            // printf("removing newline\n");
            lines[i][len - 1] = '\0'; // Remove newline character
        }
    }

    fclose(file);
    return lines;
}

void free_lines(char **lines, int num_lines)
{
    // return all the memory :)
    for (int i = 0; i < num_lines; i++)
    {
        free(lines[i]);
    }
    free(lines);
}

int part_1(char **lines, const int num_lines, const int line_length)
{
    // Oh, this feels like a crappy solution, but:
    // - it is what it is
    // - it does what it should
    // - it works :)

    int count = 0;

    // horizontal
    char word[5] = {0}; // Why does this need to be 5?
    for (int y = 0; y < num_lines; y++)
    {
        for (int x = 0; x < line_length - 3; x++)
        {
            if (lines[y][x] == 'X')
            {
                strncpy(word, lines[y] + x, 4);
                if (strcmp(word, "XMAS") == 0)
                {
                    count++;
                }
            }
            else if (lines[y][x] == 'S')
            {
                strncpy(word, lines[y] + x, 4);
                if (strcmp(word, "SAMX") == 0)
                {
                    count++;
                }
            }
        }
    }

    // vertical
    for (int y = 0; y < num_lines - 3; y++)
    {
        for (int x = 0; x < line_length; x++)
        {
            if (lines[y + 0][x] == 'X' &&
                lines[y + 1][x] == 'M' &&
                lines[y + 2][x] == 'A' &&
                lines[y + 3][x] == 'S')
            {
                count++;
            }
            else if (lines[y + 0][x] == 'S' &&
                     lines[y + 1][x] == 'A' &&
                     lines[y + 2][x] == 'M' &&
                     lines[y + 3][x] == 'X')
            {
                count++;
            }
        }
    }

    // diagonal
    for (int y = 0; y < num_lines - 3; y++)
    {
        for (int x = 0; x < line_length - 3; x++)
        {
            if (lines[y + 0][x + 0] == 'X' &&
                lines[y + 1][x + 1] == 'M' &&
                lines[y + 2][x + 2] == 'A' &&
                lines[y + 3][x + 3] == 'S')
            {
                count++;
            }
            else if (lines[y + 0][x + 0] == 'S' &&
                     lines[y + 1][x + 1] == 'A' &&
                     lines[y + 2][x + 2] == 'M' &&
                     lines[y + 3][x + 3] == 'X')
            {
                count++;
            }
        }
    }

    // other diagonal
    for (int y = 3; y < num_lines; y++)
    {
        for (int x = 0; x < line_length - 3; x++)
        {
            if (lines[y - 0][x + 0] == 'X' &&
                lines[y - 1][x + 1] == 'M' &&
                lines[y - 2][x + 2] == 'A' &&
                lines[y - 3][x + 3] == 'S')
            {
                count++;
            }
            else if (lines[y - 0][x + 0] == 'S' &&
                     lines[y - 1][x + 1] == 'A' &&
                     lines[y - 2][x + 2] == 'M' &&
                     lines[y - 3][x + 3] == 'X')
            {
                count++;
            }
        }
    }
    return count;
}

int part_2(char **lines, const int num_lines, const int line_length)
{
    // LOL, while part 1 felt like this was a crappy way of doing things,
    // for part 2 instead it just feels right :D

    int count = 0;

    for (int y = 1; y < num_lines - 1; y++)
    {
        for (int x = 1; x < line_length - 1; x++)
        {
            if (lines[y + 0][x + 0] == 'A' &&
                ((lines[y - 1][x - 1] == 'M' && lines[y + 1][x + 1] == 'S') ||
                 (lines[y - 1][x - 1] == 'S' && lines[y + 1][x + 1] == 'M')) &&
                ((lines[y + 1][x - 1] == 'M' && lines[y - 1][x + 1] == 'S') ||
                 (lines[y + 1][x - 1] == 'S' && lines[y - 1][x + 1] == 'M')))
            {
                count++;
            }
        }
    }

    return count;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    int num_lines;
    int line_length;

    char **lines = read_file(argv[1], &num_lines, &line_length);

    int sum1 = part_1(lines, num_lines, line_length);
    printf("part 1: %d\n", sum1);

    int sum2 = part_2(lines, num_lines, line_length);
    printf("part 2: %d\n", sum2);

    free_lines(lines, num_lines);

    return 0;
}