#include "stdio.h"
#include "defs.h"

int main(int argc, char** argv){
    
    if(argc != 2){
        printf("Usage <Program> <File>");
        return 1;
    }
    
    uint16_t isBigEndian = 0;
    uint32_t numFiles = 1; // TODO: When we have directory support, this must be based on the files in argv[1]
    
    CRN_Header crnHdr = {0};
    crnHdr.Magic[0] = 'C';
    crnHdr.Magic[1] = 'R';
    crnHdr.Magic[2] = 'N';
    crnHdr.Magic[3] = '\0';
    crnHdr.Endianness = isBigEndian; 
    crnHdr.NumFiles = numFiles;
    
    FILE *crnFile = fopen("test.crn", "wb");
    CRN_Header *ptr = &crnHdr;
    fwrite(ptr, sizeof(CRN_Header), 1, crnFile);
    fclose(crnFile);
    
    //Will be wrapped in a for loop
    crnFile = fopen("test.crn", "ab"); //To append
    FILE *toPack = fopen(argv[1], "r");
    fseek(toPack, 0L, SEEK_END);
    long sz = ftell(toPack);
    fseek(toPack, 0L, SEEK_SET);
    fwrite(toPack, sz, 1, crnFile);
    fclose(toPack);
    fclose(crnFile);
    return 0;
}