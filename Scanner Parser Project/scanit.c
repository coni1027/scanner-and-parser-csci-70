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
    if (openScanner(argv[1]) == 0) {
        fprintf(stderr,"Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    // Makes a string for scanner output file name
    char fileString[300];
    char *lastPeriod = strrchr(argv[1],'.');

    // Checks if lastPeriod exists, if not write from the last, if yes write before the period
    int baseLength = lastPeriod ? (int)(lastPeriod - argv[1]) : (int)strlen(argv[1]);

    // Writes from the period or from end of file
    snprintf(fileString, sizeof(fileString), "%.*s_scanner_output.txt", baseLength,argv[1]);

    // Writes the file
    FILE *writeFile = fopen(fileString, "w");

    // Safeguard in writing file
    if (writeFile == NULL) {
        fprintf(stderr, "Error: Could not create file %s\n", fileString);
        closeScanner();
        return 1;
    }

    // Main loop
    struct token t = gettoken();
    while (t.id != TokenEndOfFile) {
        // printf("%s %s\n", tokennames[t.id], t.lexeme);
        fprintf(writeFile,"%s %s\n", tokennames[t.id], t.lexeme);

        t = gettoken();
    }

    fclose(writeFile);
    closeScanner();
    
    return 0;
}