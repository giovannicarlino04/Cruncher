#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>

int get_file_size(FILE* file) {
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);  
    return size;
}

const char* get_file_extension(const char* filename) {
    if (!filename) return NULL;

    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return NULL; // No extension found

    return dot; // Return pointer to the extension
}