#include "../Interpreter/General/general.h"
#include <time.h>

#define MILLISECONDDELAY_2 20
#define SHORTDELAY 5
#define GREEN 128
#define NORTH 180
#define BLACK 0
#define PINK 238
#define VIOLET 75
#define ORANGE 165
#define L_RED 139
#define MAXRADIUS 500

enum col {white, black, red, green, yellow, orange};
typedef enum col col;

/* Very similar Do function as in Interpreter/general.c but with
different do_loop */
bool Do_ext(Program *p);
/* Very similar do_loop function as in Interpreter/general.c but
with different instrctlst */
bool do_loop_ext(Program *p, double max, char iter_var[2]);
/* Very similar assign_move function as in Interpreter/general.c but
with different new_pos fn */
bool assign_move_ext(Program *p, move m);
/* Very Similar to Move in Interpreter/general.c */
bool Move_ext(Program *p);
/* Very Similar to Instruction in Interpreter/general.c */
bool Instruction_ext(Program *p);
/* Very Similar to Instrctlst in Interpreter/general.c */
bool Instrctlst_ext(Program *p);
/* Very Similar to Main in Interpreter/general.c */
bool Main_ext(Program *p);
/* Converts a string to one of the modes, returns mode */
mode Str_to_mode(char *str);
/* Checks if the mode to be deactivated is actually active */
bool eq_mode(Program *p, mode m);
/* Returns true if able to activate / deactivate a mode */
bool ModeSelect(Program *p);
/* Returns true if valid explosion */
bool Explosion(Program *p);
/* Randomly chooses assigns a new set of x,y coordinates to struct */
bool Teleport(Program *p);
/* Displays my own teleporting animation (alternating blue and blue circles) */
void draw_teleport(Program *p);
/* Returns true if a MODE == TRUE/FALSE condition holds */
bool true_mode(Program *p, mode m);
/* Returns true if current word is valid varnum and is equal to number in var */
bool true_var(Program *p, char *var);
/* MODE part of Condition. Returns true if == is entered, and condition holds */
bool condition_mode(Program *p, mode m);
/* VARNUM part of Condition. Returns true if == is entered, and condition holds */
bool condition_varnum(Program *p);
/* Depending on rainbowcnt in structure, assigns a colour in rainbow */
void rainbow_set(Program *p);
/* Draws a line in SDL - incorporates drawing mode */
void draw_line_ext(Program *p);
/* Puts p->cw at the correct place if the IF condition doesn't hold.
Source: https://www.geeksforgeeks.org/check-for-balanced-parentheses-
in-an-expression/ */
void update_currentwd(Program *p);
/* SDL own animation - draws an explosion with centre at current x,y */
void draw_explosion(Program *p, int r, col c);
/* Checking that a valid condition for IF is entered, and it is true */
bool Condition(Program *p);
/* Returns true if valid IF statement instruction */
bool If_statement(Program *p);
/* Returns true if valid Varnum and Move used */
bool valid_movesize_ex(Program *p, move m);
/* Assertion Tests */
void test(void);
