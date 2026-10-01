/* PROTOTYPES FOR MAIN FUNCTIONS OF SCAN.C */

#include "token.h"
#define FALSE 0
#define TRUE 1

// Opens file, returns 0 on failure, 1 on success
int openScanner(char *filename);

// Closes file, reset line number to 1, clear pushback
// For handling multiple files
void closeScanner();

// Helper function in getting the line number of file read
int getLineNumber();

// Message of the most recent Error token
const char *getErrorMessage();

// Line where the most recent token started
int getTokenLine();

// Specs explicitly said gettoken
// why is it not in camel case fml
struct token gettoken();