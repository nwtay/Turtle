#include "../General/general.h"

struct prog {
   char **words;
   unsigned int numwords;
   /*Counter for current word*/
   unsigned int cw;
   /*Flag to signify currently testing*/
   bool tflag;
};

typedef struct prog Program;

/* Returns true if Move instruction parsed */
bool Move(Program *p);
/*Returns true if managed to parse Varnum correctly*/
bool Instruction(Program *p);
/*Returns true if valid INSTRCTLST*/
bool Instrctlst(Program *p);
/*Returns true if valid MAIN*/
bool Main_gram(Program *p);
/* Returns true if valid polish expression */
bool Polish(Program *p);
/* Returns true if valid set expression */
bool Set(Program *p);
/* Returns true if valid do expression */
bool Do(Program *p);
/* Assertion Tests */
void test(void);
/* Prints out error to stderr if invalid expression */
void conditional_err(Program *p, char *err);
/* Frees everything at the end */
void free_all(Program *p);
/* Initialises the structure */
Program* prog_init(unsigned int word_cnt);
/* Reads in the file */
Program* ReadFile(int argc, char *textfile);
