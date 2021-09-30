#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "../ADTs/General/general.h"
#include "../ADTs/General/bool.h"

#define MAXNUMTOKENS 500
#define MAXTOKENLEN 100
#define MAXHEIGHT 10000
#define MAXWIDTH 10000
#define ALPHA 26

/*Returns true if input character is a VAR, i.e. within A-Z*/
bool is_var(char* c);
/*Returns true if input character is an OPERATOR, i.e. +, -, *, /*/
bool is_op(char* c);
/*Returns true if character is a NUMBER*/
bool is_num(char* c);
/* Checks the test flag in structure, and prints out error message */
bool Varnum(char *c);
/* Goes through the file, and records the number of words, used for callocing */
unsigned int word_count(char *textfile);
