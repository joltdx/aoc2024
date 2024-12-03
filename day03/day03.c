#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int part1(const char *line)
{
    regex_t regex;
    regmatch_t matches[2];
    const char *pattern = "mul\\([0-9]{1,3},[0-9]{1,3}\\)";
    int status;
    char match[12];
    int sum = 0;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    const char *start = line;
    while (1)
    {
        status = regexec(&regex, start, 2, matches, 0);
        if (status != 0)
        {
            break;
        }

        int match_start = matches[0].rm_so;
        int match_end = matches[0].rm_eo;
        int match_length = match_end - match_start;
        strncpy(match, start + match_start, match_length);
        match[match_length] = '\0';
        start += match_end;

        int left = 0;
        int right = 0;
        if (sscanf(match, "mul(%d,%d)", &left, &right) == 2)
        {
            sum += left * right;
        }
    }

    return sum;
}

int part2(const char *line)
{
    regex_t regex;
    regmatch_t matches[2];
    const char *pattern = "mul\\([0-9]{1,3},[0-9]{1,3}\\)|do\\(\\)|don't\\(\\)";
    int status;
    char match[12];
    int sum = 0;
    int enabled = 1;

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    const char *start = line;
    while (1)
    {
        status = regexec(&regex, start, 2, matches, 0);
        if (status != 0)
        {
            break;
        }

        int match_start = matches[0].rm_so;
        int match_end = matches[0].rm_eo;
        int match_length = match_end - match_start;
        strncpy(match, start + match_start, match_length);
        match[match_length] = '\0';
        start += match_end;
        if (strcmp(match,"do()") == 0)
            enabled = 1;
        if (strcmp(match,"don't()") == 0)
            enabled = 0;

        if (enabled == 1)
        {
            int left = 0;
            int right = 0;
            if (sscanf(match, "mul(%d,%d)", &left, &right) == 2)
            {
                sum += left * right;
            }
        }
    }

    return sum;
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
        exit(1);
    }

    char *buffer;
    long filesize;

    // Seek to end of file, and get file pointer (= file size at that position)
    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    rewind(file); // Rewinding file to start again
    if (filesize == -1)
    {
        fprintf(stderr, "Could not get file size\n");
        fclose(file);
        exit(1);
    }
    // allocate memory, filesize + 1 for null byte
    buffer = (char *)malloc(filesize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        exit(1);
    }

    // read the whole file
    size_t fread_size = fread(buffer, 1, filesize, file);
    if (fread_size != filesize)
    {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        exit(1);
    }

    // null-terminate the buffer
    buffer[filesize] = '\0';

    int sum1 = part1(buffer);
    int sum2 = part2(buffer);
    printf("part 1: %d\npart 2: %d\n", sum1, sum2);

    free(buffer);
    fclose(file);
    return 0;
}