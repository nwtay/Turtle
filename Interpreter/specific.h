#include "General/general.h"

/*Returns true if a manoeuvre (FD, LT or RT) is implemented correctly*/
bool Move_interp(Program *p);
/*Returns true if valid INSTRUCTION*/
bool Instruction_interp(Program *p);
/*Returns true if valid INSTRCTLST*/
bool Instrctlst_interp(Program *p);
/*Returns true if valid MAIN*/
bool Main_interp(Program *p);
/* Does the Do instruction - interpreter version */
bool Do_interp(Program *p);
/* Checks whether the instruction set is valid, and performs DO loop */
bool do_loop(Program *p, double max, char iter_var[2]);
/* Checks that d1 is not zero when dividing */
bool zero_check(Program *p, char *str, double d1);
/* Returns true if successfully completed a move-type instruction */
bool assign_move(Program *p, move m);
/* Draws a line in SDL from old x,y coordinates to new x,y coordinates */
void draw_line(Program *p);
/* Returns true if valid Varnum and Move used */
bool valid_movesize(Program *p, move m);
/* Assertion testing */
void test(void);
