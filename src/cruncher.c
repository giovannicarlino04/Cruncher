#include "cruncher.h"
#include "file_utils.h"
#include <stdio.h>
#include "cruncher.h"
#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>

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