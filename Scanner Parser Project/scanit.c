/* SCANNER */

#include <stdio.h>
#include <string.h>
#include "scan.h"

int main(int argc, char *argv[]) {
    // Safeguard
    if (argc < 2) {
        fprintf(stderr,"Error: No arguments passed\n");
        return 1;
    } 
    
    // For debugging, will remove later
    // printf("Arguments passed: %s\n", argv[1]);
    
    // Opens file
    if (openFile(argv[1]) == 0) {
        fprintf(stderr,"Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    // Main loop
    struct token t = gettoken();
    while (t.id != TokenEndOfFile) {
        printf("%s %s\n", tokennames[t.id], t.lexeme);

        t = gettoken();
    }

    closeFile();
    
    return 0;
}