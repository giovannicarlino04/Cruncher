#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include "cruncher.h"
#include "file_utils.h"

int cruncher_pack_folder(const char* sDir){
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    
    char sPath[2048];
    sprintf(sPath, "%s\\*.*", sDir);
    
    if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
        printf("Path not found: [%s]\n", sDir);
        return -1;
    }
    
    int numFiles = list_dir_contents(sDir);  
    
    
    CRN_Header crnHdr = {0};
    crnHdr.Magic[0] = 'C';
    crnHdr.Magic[1] = 'R';
    crnHdr.Magic[2] = 'N';
    crnHdr.Magic[3] = '\0';
    crnHdr.Version = 1;
    crnHdr.Endianness = 0;  // Assuming little-endian (cuz why not)
    crnHdr.NumFiles = numFiles;
    
    const char* fileName = "Out.crn";
    FILE *crnFile = fopen(fileName, "wb");
    if (!crnFile) {
        perror("Failed to open output file");
        return -1;
    }
    
    fwrite(&crnHdr, sizeof(CRN_Header), 1, crnFile);
    fclose(crnFile);
    
    crnFile = fopen(fileName, "ab");
    if (!crnFile) {
        perror("Failed to open output file for appending");
        return -1;
    }
    
    do {
        if(strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0) {
            sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);
            
            
            FILE *toPack = fopen(sPath, "rb");
            if (!toPack) {
                perror("Failed to open input file");
                continue; 
            }
            
            
            int size = get_file_size(toPack);
            if (size <= 0) {
                perror("Invalid file size");
                fclose(toPack);
                continue;
            }
            
            void *buf = malloc(size);
            if (!buf) {
                perror("Memory allocation failed");
                fclose(toPack);
                continue;
            }
            
            
            if (fread(buf, 1, size, toPack) != size) {
                perror("File read failed");
                free(buf);
                fclose(toPack);
                continue;
            }
            
            
            if (fwrite(buf, 1, size, crnFile) != size) {
                perror("File write failed");
            }
            
            free(buf);
            fclose(toPack);
        }
    } while (FindNextFile(hFind, &fdFile));
    
    FindClose(hFind);
    fclose(crnFile);
    return 0;
}


int cruncher_unpack(const char *crnFilePath, const char *outDir) {
    FILE *crnFile = fopen(crnFilePath, "rb");
    if (!crnFile) {
        perror("Failed to open .crn file");
        return -1;
    }
    
    // Read the CRN header
    CRN_Header crnHdr;
    if (fread(&crnHdr, sizeof(CRN_Header), 1, crnFile) != 1) {
        perror("Failed to read header from .crn file");
        fclose(crnFile);
        return -1;
    }
    
    // Validate the magic number
    if (crnHdr.Magic[0] != 'C' || crnHdr.Magic[1] != 'R' || crnHdr.Magic[2] != 'N' || crnHdr.Magic[3] != '\0') {
        printf("Invalid .crn file format\n");
        fclose(crnFile);
        return -1;
    }
    
    // Debug: Output the number of files
    printf("Number of files in .crn: %d\n", crnHdr.NumFiles);
    
    // Create the output directory if it doesn't exist
    if (create_directory_recursive(outDir) != 0) {
        perror("Failed to create output directory");
        fclose(crnFile);
        return -1;
    }
    
    // Process each file in the .crn archive
    for (int i = 0; i < crnHdr.NumFiles; i++) {
        char fileName[256];
        
        // Read the filename (null-terminated)
        if (fread(fileName, 1, sizeof(fileName), crnFile) != sizeof(fileName)) {
            perror("Failed to read file name");
            fclose(crnFile);
            return -1;
        }
        FILE* ptr = fopen(fileName, "r");
        int fileSize = get_file_size(ptr);
        fclose(ptr);
        // Debugging: Print file name and size
        printf("Extracting: %s (size: %d bytes)\n", fileName, fileSize);
        
        // Allocate memory for the file content
        void *fileBuffer = malloc(fileSize);
        if (!fileBuffer) {
            perror("Memory allocation failed");
            fclose(crnFile);
            return -1;
        }
        
        // Read the file content
        size_t bytesRead = fread(fileBuffer, 1, fileSize, crnFile);
        if (bytesRead != fileSize) {
            printf("Error: Read %zu bytes, expected %d bytes\n", bytesRead, fileSize);
            perror("Failed to read file content");
            free(fileBuffer);
            fclose(crnFile);
            return -1;
        }
        
        // Create the output file path
        char fullFilePath[2048];
        sprintf(fullFilePath, "%s\\%s", outDir, fileName);
        
        // Open the output file for writing
        FILE *outFile = fopen(fullFilePath, "wb");
        if (!outFile) {
            perror("Failed to open output file");
            free(fileBuffer);
            fclose(crnFile);
            return -1;
        }
        
        // Write the content to the output file
        size_t bytesWritten = fwrite(fileBuffer, 1, fileSize, outFile);
        if (bytesWritten != fileSize) {
            printf("Error: Wrote %zu bytes, expected %d bytes\n", bytesWritten, fileSize);
            perror("Failed to write to output file");
            free(fileBuffer);
            fclose(outFile);
            fclose(crnFile);
            return -1;
        }
        
        // Clean up
        free(fileBuffer);
        fclose(outFile);
    }
    
    // Close the .crn file
    fclose(crnFile);
    return 0;
}


int DEBUG_cruncher_pack_single_file(const char* input_file) {
    const char* debug_fileName = "debug.crn";
    uint16_t isBigEndian = 0;
    uint32_t numFiles = 1;
    
    CRN_Header crnHdr = {0};
    crnHdr.Magic[0] = 'C';
    crnHdr.Magic[1] = 'R';
    crnHdr.Magic[2] = 'N';
    crnHdr.Magic[3] = '\0';
    crnHdr.Version = 1;
    crnHdr.Endianness = isBigEndian;
    crnHdr.NumFiles = numFiles;
    
    FILE *crnFile = fopen(debug_fileName, "wb");
    if (!crnFile) {
        perror("Failed to open output file");
        return -1;
    }
    
    fwrite(&crnHdr, sizeof(CRN_Header), 1, crnFile);
    fclose(crnFile);
    
    crnFile = fopen(debug_fileName, "ab");
    FILE *toPack = fopen(input_file, "rb");
    if (!toPack || !crnFile) {
        perror("Failed to open input or output file");
        return -1;
    }
    
    int size = get_file_size(toPack);
    if (size <= 0) {
        perror("Invalid file size");
        fclose(toPack);
        fclose(crnFile);
        return -1;
    }
    
    void* buf = malloc(size);
    if (!buf) {
        perror("Memory allocation failed");
        fclose(toPack);
        fclose(crnFile);
        return -1;
    }
    
    if (fread(buf, 1, size, toPack) != size) {
        perror("File read failed");
        free(buf);
        fclose(toPack);
        fclose(crnFile);
        return -1;
    }
    
    if (fwrite(buf, 1, size, crnFile) != size) {
        perror("File write failed");
    }
    
    free(buf);
    fclose(toPack);
    fclose(crnFile);
    
    return 0;
}

int DEBUG_cruncher_unpack_single_file(const char* input_file) {
    
    FILE *crnFile = fopen(input_file, "rb");
    if (!crnFile) {
        perror("Failed to open input file");
        return -1;
    }
    CRN_Header crnHdr;
    if (fread(&crnHdr, sizeof(CRN_Header), 1, crnFile) != 1) {
        perror("Failed to read CRN header");
        fclose(crnFile);
        return -1;
    }
    if (crnHdr.Magic[0] != 'C' || crnHdr.Magic[1] != 'R' || crnHdr.Magic[2] != 'N' || crnHdr.Magic[3] != '\0') {
        fprintf(stderr, "Invalid CRN file format\n");
        fclose(crnFile);
        return -1;
    }
    if (crnHdr.Version != 1) {
        fprintf(stderr, "Unsupported CRN version: %d\n", crnHdr.Version);
        fclose(crnFile);
        return -1;
    }
    if (crnHdr.Endianness != 0) {
        fprintf(stderr, "Unsupported endianness: %d\n", crnHdr.Endianness);
        fclose(crnFile);
        return -1;
    }
    if (crnHdr.NumFiles != 1) {
        fprintf(stderr, "Unsupported number of files: %d\n", crnHdr.NumFiles);
        fclose(crnFile);
        return -1;
    }
    char output_file_name[256];
    snprintf(output_file_name, sizeof(output_file_name), "%s_unpacked", input_file);
    FILE *outputFile = fopen(output_file_name, "wb");
    if (!outputFile) {
        perror("Failed to open output file");
        fclose(crnFile);
        return -1;
    }
    fseek(crnFile, sizeof(CRN_Header), SEEK_SET);
    int size = get_file_size(crnFile);
    if (size <= 0) {
        perror("Invalid file size");
        fclose(crnFile);
        fclose(outputFile);
        return -1;
    }
    void* buf = malloc(size);
    if (!buf) {
        perror("Memory allocation failed");
        fclose(crnFile);
        fclose(outputFile);
        return -1;
    }
    if (fread(buf, 1, size, crnFile) != size) {
        perror("File read failed");
        free(buf);
        fclose(crnFile);
        fclose(outputFile);
        return -1;
    }
    fwrite(buf, 1, size, outputFile);
    free(buf);
    fclose(crnFile);
    fclose(outputFile);
    return 0;
}