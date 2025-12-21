#include <stdio.h>
#include <stdlib.h>

#include "read.h"
#include "debug.h"
#include "utils.h"

char* read_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) printf_error("File not found: %s", filename);

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = malloc(file_size + 1);
    if (!data) {
        fclose(file);
        printf_error("Not enough memory to allocate %zu", file_size + 1);
    }

    size_t data_size = fread(data, 1, file_size, file);
    data[data_size] = '\0';
    fclose(file);
    return data;
}
