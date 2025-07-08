#include <stdio.h>
#include "src/cruncher.h"
#include "src/file_utils.h"

int main(int argc, char** argv){
    
    if(argc != 2){
        printf("Usage: <Program> <File>");
        return 1;
    }
    printf("Packing %s to debug.crn\n", argv[1]);
    
    if(argv[1] == NULL){
        printf("No file specified.\n");
        return 1;
    }
    
    if(get_file_extension(argv[1]) == "crn")
        DEBUG_cruncher_unpack_single_file(argv[1]);
    else
        DEBUG_cruncher_pack_single_file(argv[1]);
    return 0;
}