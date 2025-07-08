#include "file_utils.h"
#include <stdlib.h>
#include <Windows.h>


bool is_dir(const char* dirName_in)
{
    DWORD ftyp = GetFileAttributesA(dirName_in);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;   // Dir
    
    return false;    // File
}

bool list_dir_contents(const char *sDir)
{
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    
    char sPath[2048];
    sprintf(sPath, "%s\\*.*", sDir);
    if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: [%s]\n", sDir);
        return false;
    }
    
    do
    {
        if(strcmp(fdFile.cFileName, ".") != 0
           && strcmp(fdFile.cFileName, "..") != 0)
        {
            sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);
            
            if(fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
                printf("Directory: %s\n", sPath);
                list_dir_contents(sPath);
            }
            else{
                printf("File: %s\n", sPath);
            }
        }
    }
    while(FindNextFile(hFind, &fdFile)); //Find the next file.
    
    FindClose(hFind); //Always, Always, clean things up!
    
    return true;
}


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

int create_directory_recursive(const char *path) {
    char tmp[2048];
    char *p = NULL;
    size_t len;
    
    // Copy the path into tmp
    strcpy(tmp, path);
    len = strlen(tmp);
    
    // Remove trailing slash (if any)
    if (tmp[len - 1] == '\\' || tmp[len - 1] == '/') {
        tmp[len - 1] = '\0';
    }
    
    // Create all directories in the path
    for (p = tmp + 1; *p; p++) {
        if (*p == '\\' || *p == '/') {
            *p = '\0';
            
            // Debugging output: Print each intermediate directory
            printf("Creating directory: %s\n", tmp);
            
            // Try creating the directory
            if (CreateDirectory(tmp, NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS) {
                // If it failed and the error is not that the directory already exists, return failure
                return -1;
            }
            
            *p = '\\';  // Restore the path separator
        }
    }
    
    // Now create the final directory
    return CreateDirectory(tmp, NULL) || GetLastError() == ERROR_ALREADY_EXISTS ? 0 : -1;
}
