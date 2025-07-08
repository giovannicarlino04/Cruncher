#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>

int get_file_size(FILE* file);
const char*  get_file_extension(const char* filename);

#endif // FILE_UTILS_H