#include "../../ADTs/Stack/stack.h"
#include "../../SDL/neillsdl2.h"
#include "../../General/general.h"
#include <math.h>

#define PI 3.14159265
#define MILLISECONDDELAY 60
#define WHITE 255
#define NORTH 180

/* For extension */
enum mode {normal, ghost, fast, slow, rainbow, invalid};
typedef enum mode mode;
enum move {none, fd, lt, rt};
typedef enum move move;

struct turt {
   double x;
   double y;
   /* Current direction from North, in degrees  */
   double theta;
   /* Previous coordinates for line drawing */
   double prev_x;
   double prev_y;
   /* Extension */
   mode m;
   int rainbow_cnt;
};

typedef struct turt Turtle;

struct prog {
   char **words;
   unsigned int numwords;
   /*Counter for current word*/
   unsigned int cw;
   /*Flag to signify currently testing*/
   bool tflag;
   Turtle* turt;
   /* Histogram for set variables, A-Z*/
   double vars[ALPHA];
   /* Bool for variables, T if been set */
   bool var_set[ALPHA];
   double new_var_value;
   char* var_to_be_set;
   stack* s;
   char* iter_var;
   SDL_Simplewin sw;
};

typedef struct prog Program;
enum comp {e, l, g};
typedef enum comp comp;

/* Sets up for SDL */
void SDL_Setup(Program *p);
/* Frees everything at the end */
void free_all(Program *p);
/* Prints out error message to stderr if test flag is off */
void conditional_err(Program *p, char *err);
/* Does the Set instruction, returns true if completed successfully */
bool Set_interp(Program *p);
/* Compares two doubles */
comp double_cmp(double d1, double d2);
/* If a Varnum is accessible - i.e. if set or a number */
bool access_vnum(Program *p, char* vnum);
/* True if a variable has been set */
bool is_set_var(Program *p, char* var);
/* Checks the FROM and TO value in DO: if <, return false */
bool check_endpoints(double start, double max);
/* Value now has if statment: if is a number, then just return that number */
double value(Program *p, char *var);
/* Returns true if changed the direction of current turtle */
bool new_angle(Program *p, move m, double angle);
/* Sets the value in the histogram of a variable */
void set_value(Program *p, char* var, double value);
/* Returns the number of items on the stack */
int count_stack(stack *s);
/* Performs Polish instruction, returns true if parsed and interpreted */
bool Polish_interp(Program *p);
/* Perform the operation if instructed to do so correctly */
bool Polish_oper(Program *p);
/* Checks that d1 is not zero when dividing */
bool zero_err(Program *p, char *str, double d1);
/* Reads in the file into the 2d string array */
Program* ReadFile(int argc, char *textfile);
/* Initialises the structure */
Program* prog_init(unsigned int word_cnt);
/* Pushes valid varnum to stack and continues reading polish expr */
bool Polish_vnum(Program *p);
/* Returns result of the operation on two doubles */
double result_op(double d1, double d2, char* op);
/* Returns true if changed the direction of current turtle */
bool new_pos(Program *p, double size);
