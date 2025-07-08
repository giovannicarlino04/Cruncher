#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stdbool.h>

bool is_dir(const char* dirName_in);
bool list_dir_contents(const char *sDir);
int get_file_size(FILE* file);
const char*  get_file_extension(const char* filename);
int create_directory_recursive(const char *path);

#endif // FILE_UTILS_H