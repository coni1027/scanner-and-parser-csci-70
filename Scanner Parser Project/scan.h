/* PROTOTYPES FOR MAIN FUNCTIONS OF SCAN.C */

#include "token.h"
#define FALSE 0
#define TRUE 1

// Opens file, returns 0 on failure, 1 on success
int openFile(char *filename);

// Closes file, reset line number to 1, clear pushback
// For handling multiple files
void closeFile();

// Helper function in getting the line number of file read
int getLineNumber();

// Specs explicitly said gettoken
// why is it not in camel case fml
struct token gettoken();




