/* PARSER */

#include <stdio.h>
#include <string.h>
#include "scan.h"
#include "parse.h"

int main(int argc, char *argv[]) {
    // Safeguard
    if (argc < 2) {
        fprintf(stderr,"Error: No arguments passed\n");
        return 1;
    }

    if (openScanner(argv[1]) == 0) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    // Makes a string for parser output file name
    char fileString[300];
    char *lastPeriod = strrchr(argv[1],'.');

    // Checks if lastPeriod exists, if not write from the last, if yes write before the period
    int baseLength = lastPeriod ? (int)(lastPeriod - argv[1]) : (int)strlen(argv[1]);

    // Writes from the period or from end of file
    snprintf(fileString, sizeof(fileString), "%.*s_parser_output.txt", baseLength,argv[1]);

    // Writes the file
    FILE *writeFile = fopen(fileString, "w");

    // Safeguard in writing file
    if (writeFile == NULL) {
        fprintf(stderr, "Error: Could not create file %s\n", fileString);
        closeScanner();
        return 1;
    }
 
    parseFile(argv[1],writeFile);
 
    fclose(writeFile);
    closeScanner();

    return 0;
}