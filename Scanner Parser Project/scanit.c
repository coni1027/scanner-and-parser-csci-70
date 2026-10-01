/* SCANNER */

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "scan.h"


// Returns 1 if name is an input file: ends in ".txt" and is not one of our output files
static int isInputFile(const char *name) {
    size_t len = strlen(name);
 
    if (len < 4 || strcmp(name + len - 4, ".txt") != 0) return 0;    // not a .txt file
    if (strstr(name, "_output") != NULL) return 0;                   // own output file
 
    return 1;
}
 
// Scans one input file and writes its tokens to <name>_scanner_output.txt
// Returns 1 on success, 0 on failure
static int scanFile(char *filename) {
    // Opens input file
    if (openScanner(filename) == 0) {
        fprintf(stderr,"Error: Could not open file %s\n", filename);
        return 0;
    }
 
    // Makes a string for scanner output file name
    char fileString[300];
    char *lastPeriod = strrchr(filename,'.');
 
    // Checks if lastPeriod exists, if not write from the last, if yes write before the period
    int baseLength = lastPeriod ? (int)(lastPeriod - filename) : (int)strlen(filename);
 
    // Writes from the period or from end of file
    snprintf(fileString, sizeof(fileString), "%.*s_scanner_output.txt", baseLength, filename);
 
    // Writes the file
    FILE *writeFile = fopen(fileString, "w");
 
    // Safeguard in writing file
    if (writeFile == NULL) {
        fprintf(stderr, "Error: Could not create file %s\n", fileString);
        closeScanner();
        return 0;
    }
 
    // Main loop
    struct token t = gettoken();
    while (t.id != TokenEndOfFile) {
        // Lexical errors get their own message, other tokens print name and lexeme
        if (t.id == TokenError)
            fprintf(writeFile,"Lexical Error: %s %s (line %d)\n", getErrorMessage(), t.lexeme, getErrorLine());
        else
            fprintf(writeFile,"%s %s\n", tokennames[t.id], t.lexeme);
 
        t = gettoken();
    }
 
    fclose(writeFile);
    closeScanner();
 
    return 1;
}
 
int main(int argc, char *argv[]) {
    int i;
 
    // Files given on the command line: scan only those
    if (argc >= 2) {
        for (i = 1; i < argc; i++)
            scanFile(argv[i]);
        return 0;
    }
 
    // No files given: scan every input file in the current directory
    DIR *dir = opendir(".");
    if (dir == NULL) {
        fprintf(stderr, "Error: Could not open current directory\n");
        return 1;
    }
 
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (isInputFile(entry->d_name))
            scanFile(entry->d_name);
    }
 
    closedir(dir);
    return 0;
}