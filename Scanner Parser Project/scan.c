/* DEFINES EVERYTHING NEEDED FOR SCANNER */
/* OPTED TO NOT USE CHARACTER CLASSES */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "scan.h"

static FILE *file;
static int linenum = 1;
static int pushback = FALSE;
static int charread = '\0';

// Lexical error info for most recent Error token
static const char *errorMessage = "";
static int errorLine = 1;

// Opens file, returns 0 if failure
int openScanner(char *filename){
    file = fopen(filename,"r");

    if (file == NULL) return 0;

    return 1;
}

// Closes file, rests linenum, pushback, and last char
void closeScanner() {
    if (file != NULL) {
        fclose(file);
        file = NULL;
    }

    linenum = 1;
    pushback = FALSE;
    charread = '\0';
}

// Helper, returns linenum
int getLineNumber() {
    return linenum;
}

// Returns message for the most recent Error token
const char *getErrorMessage(){
    return errorMessage;
}

// Returns the line where the most recent Error token started
int getErrorLine() {
    return errorLine;
}

// Marks token as lexical error with a given message
static void setError(struct token *t, const char *message){
    t->id = TokenError;
    errorMessage = message;
}

// Gets character
int myGetChar() {
    if (pushback){
        pushback = FALSE;
    }

    else {
        charread = fgetc(file);
        if (charread == '\n')   linenum++;
    }
    return charread;
}

// Pushback last char for myGetChar() to return it again
static void unread() {
    pushback = TRUE;
}

// Adds character to the lexeme
static void append(struct token *t, int *len, int ch) {
    if (*len < (int)sizeof(t->lexeme)-1){
        t->lexeme[(*len)++] = (char)ch;
        
        // Properly terminates after append
        t->lexeme[*len] = '\0'; 
    }
}

// Handles states that has multiple characters by predicting next (e.g., !=, **)
static int nextIs(int expected, struct token *t, int *len){
    int ch = myGetChar();
    if (ch == expected){
        append(t, len, ch);
        return 1;
    }

    unread();
    return 0;
}

// Keyword lookup, returns identifier if not a keyword
static int keywordID(const char *s) {
    static const struct { const char *word; int id;} keywords[] = {
        {"PRINT", TokenPrint}, {"IF", TokenIf},     {"ELSE", TokenElse},
        {"ENDIF", TokenEndIf}, {"SQRT", TokenSqrt}, {"AND", TokenAnd},
        {"OR", TokenOr},       {"NOT", TokenNot}
    };

    int i;
    for (i=0; i < (int)(sizeof(keywords) / sizeof(keywords[0])); i++) {
        if (strcmp(s, keywords[i].word) == 0) {
            return keywords[i].id;
        }
    }

    return TokenIdentifier;
}

// Function to get token and indicate the lexeme
struct token gettoken() {
    struct token t;
    int len = 0;
    int ch;

    t.id = TokenError;
    t.lexeme[0] = '\0';

    // STATE 0: skip whitespace and comments until a real token starts
    // use ;; for infinite loop, break once needed
    for (;;) {
        ch = myGetChar();

        // Loops S0
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')    continue;

        // S0 to S9, divide
        if (ch == '/') {
            int next = myGetChar();
            // S9 to S10, comment
            if (next=='/') {
                do {
                    ch = myGetChar();
                }
                while (ch != '\n' && ch != EOF);
                
                // S10 to EOF
                if (ch == EOF) {
                    t.id = TokenEndOfFile;
                    return t;
                }

                // S10 to S0 since newline
                continue;
            }
            unread();
            append(&t, &len, '/');
            t.id = TokenDivide;
            return t;
        }
        break;
    }

    // S0 to EOF
    if (ch == EOF) {
        t.id = TokenEndOfFile;
        return t;
    }
    append(&t, &len, ch);

    // STATE 1: identifiers and keywords
    if (isalpha(ch) || ch == '_') {
        ch = myGetChar();
        
        // loops on S1 if its a letter, digit, and _
        while (isalnum(ch) || ch == '_') {
            append(&t, &len, ch);
            ch = myGetChar();
        }

        // checks if its a keyword, if not, return as IDEN
        unread();
        t.id = keywordID(t.lexeme);
        return t;
    }

    // STATES 2-7: numbers handling
    if (isdigit(ch)) {
        t.id = TokenNumber;
        ch = myGetChar();

        // loops on S2 if still a digit
        while (isdigit(ch)) {
            append(&t, &len, ch);
            ch = myGetChar();
        }

        // S2 to S3, decimal point
        if (ch == '.') {
            append(&t, &len, ch);
            ch = myGetChar();

            // S3 to Error if undefined input
            if (!isdigit(ch)) {
                unread();
                setError(&t, "Invalid number");
                return t;
            }

            // S3 to S4, includes loop
            while (isdigit(ch)){
                append(&t, &len, ch);
                ch = myGetChar();
            }
        }

        // S2 or S4 to S5, e/E handling
        if (ch == 'E' || ch == 'e'){
            append(&t, &len, ch);
            ch = myGetChar();

            // S5 to S6, +/- handling
            if (ch == '+' || ch == '-'){
                append(&t, &len, ch);
                ch = myGetChar();
            }

            // S5 or S6 to Error if undefined input
            if (!isdigit(ch)) {
                unread();
                setError(&t, "Invalid number");
                return t;
            }

            // S5 or S6 to S7, if digit is inputted
            while (isdigit(ch)) {
                append(&t, &len, ch);
                ch = myGetChar();
            }
        }

        // S2 or S4 or S7 to NUM once other is inputted
        unread();
        return t;

    }

    // STATE 8: Strings
    if (ch == '"') {
        ch = myGetChar();

        // S8 loop, continuously append to lexeme while " is not read yet
        while (ch != '"' && ch != '\n' && ch != '\r' && ch != EOF) {
            append(&t, &len, ch);
            ch = myGetChar();
        }

        // S8 to STR, if it reads the closing "
        if (ch == '"'){
            append(&t, &len, ch);
            t.id = TokenString; 
        }

        // S8 to Error, if newline or EOF
        else {
            unread();
            setError(&t, "Unterminated string");
        }

        return t;
    }

    // STATES 11-15: double characters and Single character tokens
    // switch case since single character only
    switch (ch) {
        case ';': t.id = TokenSemicolon;    break;
        case ',': t.id = TokenComma;        break;
        case '(': t.id = TokenLeftParen;    break;
        case ')': t.id = TokenRightParen;   break;
        case '+': t.id = TokenPlus;         break;
        case '-': t.id = TokenMinus;        break;
        case '=': t.id = TokenEqual;        break;
        case ':': t.id = nextIs('=', &t, &len) ? TokenAssign : TokenColon;          break;  // S11
        case '*': t.id = nextIs('*', &t, &len) ? TokenRaise : TokenMultiply;        break;  // S12
        case '<': t.id = nextIs('=', &t, &len) ? TokenLTEqual : TokenLessThan;      break;  // S13
        case '>': t.id = nextIs('=', &t, &len) ? TokenGTEqual : TokenGreaterThan;   break;  // S14
        case '!':
            if (nextIs('=',&t, &len))   t.id = TokenNotEqual;
            else                        setError(&t, "Invalid operator");
            break;
        default: setError(&t, "Invalid character");     break;                              // any other character, error
    }
    return t;
}