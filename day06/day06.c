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

char **copy_map(char **map, const int height, const int width) {
    char **copy = (char **)malloc(height * sizeof(char *));
    if (copy == NULL) {
        fprintf(stderr, "Error allocating memory for copy buffer\n");
            free_lines(map, height);
            exit(1);
    }
    for (int i = 0; i < height; i++) {
        copy[i] = (char *)malloc(width * sizeof(char));
        if (copy[i] == NULL) {
            fprintf(stderr, "Error allocating memory for copy index %d\n", i);
            for (int j = 0; j < i; j++)
            {
                free(copy[j]);
            }
            free(copy);
            free_lines(map, height);
            exit(1);   
        } 
        strcpy(copy[i], map[i]);
    }
    return copy;
}

void restore_map(char **map, char **copy, const int height) {
    for (int i = 0; i < height; i++) {
        strcpy(map[i], copy[i]);
    }
}

int part_1(char **map, const int height, const int width)
{
    int count = 0;

    // find start position and direction
    int x = 0;
    int y = 0;
    char d = ' ';
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            d = map[y][x];
            if (d == '^' || d == '>' || d == 'v' || d == '<')
            {
                goto leave_for_without_increasing_y; // well, learned the hard way :)
            }
        }
    }
leave_for_without_increasing_y:

    int start_x = x;
    int start_y = y;
    char start_d = d;

    // X marks the path
    while (1) { // yeah, let's just say there's always an out
        map[y][x] = 'X';
        int dx = d == '<' ? -1 : d == '>' ? 1 : 0;
        int dy = d == '^' ? -1 : d == 'v' ? 1 : 0;

        if (x + dx >= width || x + dx < 0 || y + dy >= height || y + dy < 0)
            break;

        // turn?
        char s = map[y + dy][x + dx];
        if (s == '#')
        {
            if (d == '^')
                d = '>';
            else if (d == '>')
                d = 'v';
            else if (d == 'v')
                d = '<';
            else
                d = '^';
            continue;
        }

        x = x + dx;
        y = y + dy;
    }

    // count 'X' and reset

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            if (map[y][x] == 'X')
            {
                count++;
                //map[y][x] = '.';
            }
        }
    }
    map[start_y][start_x] = start_d;

    return count;
}

int part_2(char **map, char **original_map, char **part_1_map, const int height, const int width)
{
    int count = 0;

    //char **original_map = copy_map(map, height, width);

    // find start position and direction
    int x = 0;
    int y = 0;
    char d = ' ';
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            d = map[y][x];
            if (d == '^' || d == '>' || d == 'v' || d == '<')
            {
                goto leave_for_without_increasing_y; // well, learned the hard way :)
            }
        }
    }
leave_for_without_increasing_y:

    int start_x = x;
    int start_y = y;
    char start_d = d;

    // Let's brute force this one...
    // Actually, let's optimize a bit. No need to put 'O' at all the '.',
    // just where we visited in part 1, that's optimization enough for now...

    int oy = 0;
    int ox = 0;
    int max_steps = width * height;
    for (; oy < height; oy++) {
        ox = 0;
        for (; ox < width; ox++) {
            if (part_1_map[oy][ox] != 'X') 
                continue;

            if (map[oy][ox] != '.' || (oy == start_y && ox == start_x))
                continue;
            
            map[oy][ox] = 'O';

            x = start_x;
            y = start_y;
            d = start_d;
            int step_count = 0;

            // X marks the path
            while (1) { 
                if (step_count > max_steps) {   // there's not always an out
                    count++;    // loop for sure
                    break;
                }

                map[y][x] = 'X';
                int dx = d == '<' ? -1 : d == '>' ? 1 : 0;
                int dy = d == '^' ? -1 : d == 'v' ? 1 : 0;

                if (x + dx >= width || x + dx < 0 || y + dy >= height || y + dy < 0)
                    break;

                // turn?
                char s = map[y + dy][x + dx];
                if (s == '#' || s == 'O')
                {
                    if (d == '^')
                        d = '>';
                    else if (d == '>')
                        d = 'v';
                    else if (d == 'v')
                        d = '<';
                    else
                        d = '^';
                    continue;
                }

                x = x + dx;
                y = y + dy;
                step_count++;
            }

            // reset
            restore_map(map, original_map, height);
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

    int height;
    int width;

    char **original_map = read_file(argv[1], &height, &width);

    char **part_1_map = copy_map(original_map, height, width);

    int sum1 = part_1(part_1_map, height, width);
    printf("part 1: %d\n", sum1);

    char **part_2_map = copy_map(original_map, height, width);

    int sum2 = part_2(part_2_map, original_map, part_1_map, height, width);
    printf("part 2: %d\n", sum2);

    free_lines(part_2_map, height);
    free_lines(part_1_map, height);
    free_lines(original_map, height);

    return 0;
}