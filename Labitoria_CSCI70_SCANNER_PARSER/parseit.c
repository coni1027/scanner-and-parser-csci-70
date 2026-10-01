/* PARSER */

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "scan.h"
#include "parse.h"

// Returns 1 if name is an input file: ends in ".txt" and is not one of our output files
static int isInputFile(const char *name) {
    size_t len = strlen(name);

    if (len < 4 || strcmp(name + len - 4, ".txt") != 0) return 0;   // not a .txt file
    if (strstr(name, "_output") != NULL) return 0;                   // our own output file

    return 1;
}

// Parses one input file and writes the messages to <name>_parser_output.txt
// Returns 1 on success, 0 on failure
static int parseOneFile(char *filename) {
    if (openScanner(filename) == 0) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return 0;
    }

    // Makes a string for parser output file name
    char fileString[300];
    char *lastPeriod = strrchr(filename,'.');

    // Checks if lastPeriod exists, if not write from the last, if yes write before the period
    int baseLength = lastPeriod ? (int)(lastPeriod - filename) : (int)strlen(filename);

    // Writes from the period or from end of file
    snprintf(fileString, sizeof(fileString), "%.*s_parser_output.txt", baseLength, filename);

    // Writes the file
    FILE *writeFile = fopen(fileString, "w");

    // Safeguard in writing file
    if (writeFile == NULL) {
        fprintf(stderr, "Error: Could not create file %s\n", fileString);
        closeScanner();
        return 0;
    }

    parseFile(filename, writeFile);

    fclose(writeFile);
    closeScanner();

    return 1;
}

int main(int argc, char *argv[]) {
    int i;

    // Files given on the command line: parse only those
    if (argc >= 2) {
        for (i = 1; i < argc; i++)
            parseOneFile(argv[i]);
        return 0;
    }

    // No files given: parse every input file in the current directory
    DIR *dir = opendir(".");
    if (dir == NULL) {
        fprintf(stderr, "Error: Could not open current directory\n");
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (isInputFile(entry->d_name))
            parseOneFile(entry->d_name);
    }

    closedir(dir);
    return 0;
}