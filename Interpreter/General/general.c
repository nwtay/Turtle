#include "general.h"

void SDL_Setup(Program *p)
{
   Neill_SDL_Init(&p->sw);
   Neill_SDL_SetDrawColour(&p->sw, WHITE, WHITE, WHITE);
   Neill_SDL_UpdateScreen(&p->sw);
}

void free_all(Program *p)
{
   unsigned int i = 0;
   free(p->turt);
   for(i = 0; i < p->numwords; i++){
      free(p->words[i]);
   }
   free(p->words);
   stack_free(p->s);
   free(p);
}

void conditional_err(Program *p, char *err)
{
   if(!p->tflag){
      on_error(err);
   }
}

/* source: https://math.stackexchange.com/questions/270194/
how-to-find-the-vertices-angle-after-rotation */
bool new_pos(Program *p, double size)
{
   double x = p->turt->x, rotation_x = p->turt->x, new_x;
   double y = p->turt->y, rotation_y = p->turt->y, new_y;
   double theta_rad;
   theta_rad = p->turt->theta * (M_PI/180.0);
   y += size;  /* Add the FD magnitude to y, and then rotate around rot_x, rot_y*/

   new_x = ((x-rotation_x)*cos(theta_rad)) - ((y-rotation_y)*sin(theta_rad)) \
   + rotation_x;
   new_y = ((x-rotation_x)*sin(theta_rad)) + ((y-rotation_y)*cos(theta_rad)) \
   + rotation_y;
   p->turt->x = new_x;
   p->turt->y = new_y;
   p->turt->prev_x = rotation_x;
   p->turt->prev_y = rotation_y;
   return true;
}

bool Set_interp(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "SET")){
         p->cw++;
         if(is_var(p->words[p->cw])){
            p->var_to_be_set = p->words[p->cw++];
            if(!strcmp(p->words[p->cw++], ":=")){
               if(Polish_interp(p)){
                  set_value(p, p->var_to_be_set, p->new_var_value);
                  return true;
               }
               conditional_err(p, "Expected Valid variable, number, or ;");
            }
            conditional_err(p, "Expected ':='");
         }
         conditional_err(p, "Expected Valid Variable");
      }
   }
   return false;
}

comp double_cmp(double d1, double d2)
{
   /* e = equal, g = greater that, l = less than */
   double diff, epsilon = 0.000001, n_epsilon = -0.000001;
   diff = d1 - d2;
   if(diff <= epsilon && diff >= n_epsilon){
      return e;
   }
   if(diff > epsilon){
      return g;
   }
   else{
      return l;
   }
}

bool access_vnum(Program *p, char* vnum)
{
   if(Varnum(vnum)){
      if(is_var(vnum)){
         if(!is_set_var(p, vnum)){
            return false;
         }
      }
      return true;
   }
   return false;
}

bool is_set_var(Program *p, char* var)
{
   if(is_var(var)){
      char c;
      bool b;
      int index;
      c = var[0];
      index = (int)c - 65;
      b = p->var_set[index];
      return b;
   }
   return false;
}

bool check_endpoints(double start, double max)
{
   if(double_cmp(start, max) != g){
      return true;
   }
   return false;
}

double value(Program *p, char *var)
{
   char c;
   double d;
   if(is_var(var)){
      int index;
      c = var[0];
      index = (int)c - 65;
      d = p->vars[index];
   }
   else{
      d = strtod(var, NULL);
   }
   return d;
}

bool new_angle(Program *p, move m, double angle)
{
   if(m){
      if(m == lt){
         p->turt->theta += angle;
         return true;
      }
      if(m == rt){
         p->turt->theta -= angle;
         return true;
      }
   }
   return false;
}

void set_value(Program *p, char* var, double value)
{
   char c;
   int index;
   c = var[0];
   index = (int)c - 65;
   p->vars[index] = value;
   p->var_set[index] = true;
}

int count_stack(stack *s)
{
   return s->size;
}

bool Polish_interp(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], ";")){
         if(count_stack(p->s)==1){
            stack_pop(p->s, &p->new_var_value);
            return true;
         }
         conditional_err(p, "Invalid Polish: Too many items left on stack");
         return false;
      }
      if(Polish_oper(p)){
         return true;
      }
      if(Polish_vnum(p)){
         return true;
      }
   }
   conditional_err(p, "Expected Valid Variable, Number, or ;");
   return false;
}

bool Polish_oper(Program *p)
{
   double d1, d2;
   if(is_op(p->words[p->cw])){
      if(!count_stack(p->s) || !stack_pop(p->s, &d1) || !stack_pop(p->s, &d2)){
         conditional_err(p, "Error: Nothing left on Stack.");
         return false;
      }
      if(!zero_err(p, p->words[p->cw], d1)){
         stack_push(p->s, result_op(d1, d2, p->words[p->cw++]));
         if(Polish_interp(p)){
            return true;
         }
      }
      conditional_err(p, "Can't divide by Zero.");
      return false;
   }
   return false;
}

bool zero_err(Program *p, char *str, double d1)
{
   if(!strcmp(str,"/")){
      if(double_cmp(d1, 0.0)==e){
         conditional_err(p, "Error: Cannot divide by Zero.");
         return true;
      }
   }
   return false;
}

Program* ReadFile(int argc, char *textfile)
{
   Program *p = NULL;
   FILE *fp = nfopen(textfile, "r");
   char tmp_wd[MAXTOKENLEN];
   unsigned int i = 0;
   if(argc <= 1){
      on_error("Didn't specify a file");
   }
   p = prog_init(word_count(textfile));
   while(fscanf(fp, "%s", tmp_wd) && i < p->numwords){
      p->words[i] = (char *)ncalloc((int)strlen(tmp_wd)+1, sizeof(char));
      if(strlen(tmp_wd)){
         strcpy(p->words[i], tmp_wd);
         assert(!strcmp(tmp_wd, p->words[i]));
         i++;
      }
   }
   fclose(fp);
   return p;
}

Program* prog_init(unsigned int word_cnt)
{
   Program* p = (Program *)ncalloc(1, sizeof(Program));
   p->words = (char**)ncalloc(word_cnt, sizeof(char *));
   p->numwords = word_cnt;
   p->s = stack_init();
   p->turt = (Turtle *)ncalloc(1, sizeof(Turtle));
   p->turt->theta = NORTH;
   p->turt->x = WWIDTH/2;
   p->turt->y = WHEIGHT/2;
   return p;
}

bool Polish_vnum(Program *p)
{
   double d1;
   if(access_vnum(p, p->words[p->cw])){
      /* Getting this variables value in array */
      d1 = value(p, p->words[p->cw++]);
      stack_push(p->s, d1);
      if(Polish_interp(p)){
         return true;
      }
   }
   return false;
}

double result_op(double d1, double d2, char* op)
{
   if(!strcmp(op, "+")){
      return d1 + d2;
   }
   else if(!strcmp(op, "-")){
      return d1 - d2;
   }
   else if(!strcmp(op, "*")){
      return d1 * d2;
   }
   else if(!strcmp(op, "/")){
      return d2 / d1;
   }
   else{
      return 0;
   }
}
