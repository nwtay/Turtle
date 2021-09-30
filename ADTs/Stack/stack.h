#include "../General/general.h"
#include "../General/bool.h"

typedef struct stack stack;
typedef double datatype;

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct dataframe {
   datatype i;
   struct dataframe* next;
};
typedef struct dataframe dataframe;

struct stack {
   /* Underlying array */
   dataframe* start;
   int size;
};

#define FORMATSTR "%f"
#define ELEMSIZE 20
#define STACKTYPE "Linked"

/* Create an empty stack */
stack* stack_init(void);
/* Add element to top */
void stack_push(stack* s, double i);
/* Take element from top */
bool stack_pop(stack* s, double* d);
/* Clears all space used */
bool stack_free(stack* s);
/* Optional? */
/* Copy top element into d (but don't pop it) */
bool stack_peek(stack*s,  datatype* d);
/* Make a string version - keep .dot in mind */
void stack_tostring(stack*, char* str);
