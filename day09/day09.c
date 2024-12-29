#include <stdio.h>
#include <stdlib.h>

int *read_ints_from_file(const char *filename, int *size) {
    FILE *file;

    file = fopen(filename, "ra");
    if (file == NULL)
    {
        fprintf(stderr, "Could not read file %s\n", filename);
        exit(1);
    }

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

    // read file contents into buffer
    char *buffer = (char *)malloc(filesize * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Could not alocate memory for file buffer\n");
        fclose(file);
        exit(1);
    }
    fread(buffer, 1, filesize, file);
    fclose(file);

    // allocate memory for ints
    int *array = (int*)malloc(filesize * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Could not alocate memory for int array\n");
        fclose(file);
        free(buffer);
        exit(1);
    }

    int index = 0;
    for (int i = 0; i < filesize; i++) {
        if (buffer[i] >= '0' && buffer[i] <= '9') {
            array[index++] = buffer[i] - '0';
        }
    }

    *size = index;
    return array;
}

int *get_layout_from_map(int *disk_map, const int map_size, int *layout_size) {
    int *layout = (int *)malloc(map_size * 10 * sizeof(int));
    if (layout == NULL) {
        fprintf(stderr, "Could not alocate memory for layout\n");
        free(disk_map);
        exit(1);
    }

    *layout_size = 0;
    int b = -1;
    int c = 0;
    for (int i = 0; i < map_size; i++ ) {
        if (i % 2 == 0) {
            b++;
            c = b;
        }
        else
            c = -1;

        for (int a = 0; a < disk_map[i]; a++)
            layout[(*layout_size)++] = c;
    }

    (*layout_size)--;

    return layout;
}

long part_1(int *disk_map, const int size) {

    int l_size = 0;
    int *layout = get_layout_from_map(disk_map, size, &l_size);

    int fc = 0;
    int bc = l_size;
    while (fc < bc) {
        if (layout[fc] == -1) {
            while (layout[bc] == -1)
                bc--;
            if (fc < bc) {
                layout[fc] = layout[bc];
                layout[bc] = -1;
            }
        }
        fc++;
    }

    long sum = 0;

    for (int i = 0; i < fc; i++)
        if (layout[i] > -1)
            sum += i * layout[i];

    return sum;
}

typedef struct {
    int id;
    int offset;
    int size;
} IndexedLayout;

IndexedLayout *get_indexed_layout_from_map(int *disk_map, const int map_size, int *layout_size) {
    IndexedLayout *layout = (IndexedLayout *)malloc(map_size * 10 * sizeof(IndexedLayout));
    if (layout == NULL) {
        fprintf(stderr, "Could not alocate memory for layout\n");
        free(disk_map);
        exit(1);
    }

    *layout_size = 0;
    int b = -1;
    int c = 0;
    int offset = 0;
    for (int i = 0; i < map_size; i++ ) {
        if (i % 2 == 0) {
            b++;
            c = b;
        }
        else
            c = -1;

        layout[(*layout_size)].offset = offset;
        layout[(*layout_size)].id = c;
        layout[(*layout_size)].size = disk_map[i];
        (*layout_size)++;
        offset += disk_map[i];
    }

    (*layout_size)--;

    return layout;
}

long part_2(int *disk_map, const int size) {

    int l_size = 0;
    IndexedLayout *layout = get_indexed_layout_from_map(disk_map, size, &l_size);

    int p_size = 0;
    int *phys_layout = get_layout_from_map(disk_map, size, &p_size);

    int bc = l_size;
    while (bc > 0) {
        if (layout[bc].id == -1) {
            bc--;
            continue;
        }

        int fc = 0;
        while((layout[fc].id > -1 || layout[fc].size < layout[bc].size) && fc < bc)
            fc++;

        if (fc == bc) {
            // no
            bc --;
            continue;
        }

        for (int i = 0; i < layout[bc].size; i++) {
            phys_layout[layout[fc].offset + i] = layout[bc].id;
            phys_layout[layout[bc].offset + i] = -1;
        }
        layout[fc].offset += layout[bc].size;
        layout[fc].size -= layout[bc].size;

        bc--;
    }

    long sum = 0;

    for (int i = 0; i <= p_size; i++)
        if (phys_layout[i] > -1)
            sum += i * phys_layout[i];

    return sum;
}

int main(int argc, char **argv) {
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input>\n", argv[0]);
        exit(1);
    }

    int size = 0;
    int *ints = read_ints_from_file(argv[1], &size);

    long result1 = part_1(ints, size);
    printf("part 1: %ld\n", result1);

    long result2 = part_2(ints, size);
    printf("part 2: %ld\n", result2);

    return 0;
}