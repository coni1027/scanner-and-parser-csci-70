/* DEFINES EVERYTHING NEEDED FOR PARSER */

#include <stdio.h>
#include "parse.h"
#include "scan.h"

// A lookahead for the next unconsumed token
static struct token tok;

// Output file for parse messages
static FILE *outputFile;

// Forward declarations for productions
static int Blk();
static int Stm();
static int Exp();

// Moves to the next token
static void advance(){
    tok = gettoken();
}

// Checks if it is the expected token, then advance
static int match(int expected){
    if (tok.id != expected) {
        fprintf(outputFile,"Symbol expected\n");
        return 0;
    }
    
    advance();
    return 1;
}
// EXPRESSIONS
// Val -> Identifier {iden}, number {num}, SQRT(Exp), (Exp)
static int Val(){
    switch (tok.id){
        case TokenIdentifier:
        case TokenNumber:
            advance();
            return 1;
        case TokenSqrt:
            return match(TokenSqrt) && match (TokenLeftParen) 
            && Exp() && match(TokenRightParen);
        default:
            return match(TokenLeftParen) && Exp() && match(TokenRightParen);
    }
}

// Lit -> -Val {minus}, Val
static int Lit(){
    if (tok.id == TokenMinus) {
        advance();
        return Val();
    }
    return Val();
}

// Litfollow -> ** Lit Litfollow {raise}
static int Litfollow(){
    while (tok.id == TokenRaise) {
        advance();
        if (!Lit()) return 0;
    }

    // the e
    return 1;
}

// Fac -> Lit Litfollow
static int Fac() {
    return Lit() && Litfollow();
}

// Facfollow -> * Fac Facfollow {mult}, / Fac Facfollow {div}, e
static int Facfollow(){
    while (tok.id == TokenMultiply || tok.id == TokenDivide){
        advance();
        if (!Fac()) return 0;
    }

    // the e
    return 1;
}

// Trm -> Fac Facfollow
static int Trm() {
    return Fac() && Facfollow();
}
 
// Trmfollow -> + Trm Trmfollow {plus}, - Trm Trmfollow {minus}, e
static int Trmfollow(void) {
    while (tok.id == TokenPlus || tok.id == TokenMinus) {
        advance();
        if (!Trm()) return 0;
    }

    // the e
    return 1; 
}
 
// Exp -> Trm Trmfollow
static int Exp() {
    return Trm() && Trmfollow();
}

// CONDITIONS
// Rel -> <,=,>,<=,!=.>=
static int Rel(){
    switch (tok.id){
        case TokenLessThan:
        case TokenEqual:
        case TokenGreaterThan:
        case TokenGTEqual:
        case TokenNotEqual:
        case TokenLTEqual:
            advance();
            return 1;
        default:
            fprintf(outputFile,"Missing relational operator\n");
            return 0;
    }
}
// Cnd -> Exp Rel Exp
static int Cnd(){
    return Exp() && Rel() && Exp();
}

// PRINT ARGS
// Arg -> string {str}, Exp
static int Arg() {
    if (tok.id == TokenString) {
        advance();
        return 1;
    }

    return Exp();
}

// Argfollow -> , Arg Argfollow {comma}, e
static int Argfollow() {
    while (tok.id == TokenComma) {
        advance();
        if (!Arg()) return 0;
    }

    // the e
    return 1;   
}

// IF STATEMENT
// Iffollow -> ENDIF; {ENDIF}, ELSE Blk ENDIF; {ELSE}
static int Iffollow(){
    switch (tok.id) {
    case TokenEndIf:
        return match(TokenEndIf) && match(TokenSemicolon);
    case TokenElse:
        if (match(TokenElse) && Blk() && match(TokenEndIf) && match(TokenSemicolon))
            return 1;

        fprintf(outputFile,"Incomplete if Statement\n");
        return 0;
    default:
        // Neither ENDIF nor ELSE: failed match
        fprintf(outputFile,"Symbol expected\n");
        return 0;
    }
}

// STATEMENTS
// Stm -> iden := Exp; {iden}, PRINT(arg argfollow); {print}, IF cnd: blk iffolow {if}
static int Stm(){
    int ok = 0;

    switch (tok.id) {
        case TokenIdentifier:
            ok = match(TokenIdentifier) && match(TokenAssign)
            && Exp() && match(TokenSemicolon);
            
            if (ok) fprintf(outputFile,"Assignment Statement Recognized\n");
            break;
 
        case TokenPrint:
            ok = match(TokenPrint) && match(TokenLeftParen)
            && Arg() && Argfollow() && match(TokenRightParen) 
            && match(TokenSemicolon);
            
            if (ok) fprintf(outputFile,"Print Statement Recognized\n");
            break;
 
        case TokenIf:
            fprintf(outputFile,"If Statement Begins\n");
            ok = match(TokenIf) && Cnd() && match(TokenColon)
            && Blk() && Iffollow();
            
            if (ok) fprintf(outputFile,"If Statement Ends\n");
            break;
 
        default:
            ok = 0;     
            break;
    }

    if (!ok) fprintf(outputFile,"Invalid Statement\n");
    return ok;
}

// Blk -> Stm Blk {Identifier, PRINT, IF}, e
static int Blk(void) {
    while (tok.id == TokenIdentifier || tok.id == TokenPrint || tok.id == TokenIf) {
        if (!Stm()) return 0;
    }

    // the e
    return 1;
}
 
// Prg -> Blk EndOfFile
static int Prg(char *filename) {
    if (Blk() && match(TokenEndOfFile)) {
        fprintf(outputFile,"%s is a valid SimpCalc program", filename);
        return 1;
    }

    fprintf(outputFile,"%s is not a valid SimpCalc program", filename);
    return 0;
}

int parseFile(char *filename, FILE *parserOutput){
    outputFile = parserOutput;
    advance();
    return Prg(filename);
}
