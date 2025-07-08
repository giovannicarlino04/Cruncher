#include <stdio.h>
#include "src/cruncher.h"
#include "src/file_utils.h"

int main(int argc, char** argv){
    
    if(argc != 2){
        printf("Usage: <Program> <Directory> || <file.crn>");
        return 1;
    }
    if(argv[1] == NULL){
        printf("No file specified.\n");
        return 1;
    }
    
    if(is_dir(argv[1]))
        cruncher_pack_folder(argv[1]);
    else
        cruncher_unpack(argv[1], "./Out");
    return 0;
}