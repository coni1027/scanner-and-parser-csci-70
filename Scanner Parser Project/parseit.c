/* PARSER */

#include <stdio.h>
#include "scan.h"
#include "parse.h"

int main(int argc, char *argv[]) {
    // Safeguard
    if (argc < 2) {
        fprintf(stderr,"Error: No arguments passed\n");
        return 1;
    }

    if (openFile(argv[1]) == 0) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return 1;
    }
 
    parseFile(argv[1]);
 
    closeFile();
    return 0;
}