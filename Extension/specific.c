#include "specific.h"

int main(int argc, char* argv[])
{
   Program* p = ReadFile(argc, argv[1]);
   SDL_Setup(p);
   srand(time(NULL));
   test();
   Main_ext(p);
   p->cw++;
   if(p->cw != p->numwords){
      on_error("Error: Text after Main.");
   }
   while(!p->sw.finished){
      Neill_SDL_Events(&p->sw);
   }
   free_all(p);
   return 0;
}

bool Main_ext(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "{")){
         p->cw++;
         if(Instrctlst_ext(p)){
            return true;
         }
      }
   }
   conditional_err(p, "Expected a {");
   return false;
}

bool Instrctlst_ext(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "}") || !strcmp(p->words[p->cw], "]")){
         return true;
      }
      if(Instruction_ext(p)){
         p->cw++;
         if(Instrctlst_ext(p)){
            return true;
         }
      }
   }
   conditional_err(p, "Expected a }, or Valid Instruction");
   return false;
}

mode Str_to_mode(char *str)
{
   if(!strcmp(str, "GHOST")){
      return ghost;
   }
   if(!strcmp(str, "FAST")){
      return fast;
   }
   if(!strcmp(str, "SLOW")){
      return slow;
   }
   if(!strcmp(str, "RAINBOW")){
      return rainbow;
   }
   if(!strcmp(str, "NORMAL")){
      return normal;
   }
   else{
      return invalid;
   }
}

bool ModeSelect(Program *p)
{
   mode m;
   if(strlen(p->words[p->cw])){
      if(strcmp(p->words[p->cw], "ACTIVATE") && strcmp(p->words[p->cw], "DEACTIVATE")){
         return false;
      }
      if(!strcmp(p->words[p->cw], "ACTIVATE")){
         p->cw++;
         if((m = Str_to_mode(p->words[p->cw])) != invalid){
            p->turt->m = m;
            return true;
         }
         conditional_err(p, "Invalid Mode.");
         return false;
      }
      if(!strcmp(p->words[p->cw++], "DEACTIVATE")){
         if((m = Str_to_mode(p->words[p->cw])) != invalid){
            if(eq_mode(p, m)){
               return true;
            }
         }
         conditional_err(p, "Invalid Mode.");
      }
   }
   return false;
}

bool eq_mode(Program *p, mode m)
{
   if(m == normal){
      conditional_err(p, "Cannot Deactivate Normal");
      return false;
   }
   if(m == p->turt->m){
      p->turt->m = normal;
      return true;
   }
   conditional_err(p, "Mode was not Activated.");
   return false;
}

bool Explosion(Program *p)
{
   int i = 1;
   if(!strcmp(p->words[p->cw], "EXPLOSION")){
      if(!p->tflag){
         while(i <= MAXRADIUS){
            SDL_Delay(SHORTDELAY);
            draw_explosion(p, i, red);
            SDL_Delay(SHORTDELAY);
            draw_explosion(p, i, yellow);
            SDL_Delay(SHORTDELAY);
            draw_explosion(p, i, orange);
            i += 10;
         }
         SDL_Delay(SHORTDELAY);
         draw_explosion(p, i, black);
      }
      return true;
   }
   return false;
}

void draw_explosion(Program *p, int r, col c)
{
   switch(c) {
      case red:
         Neill_SDL_SetDrawColour(&p->sw, L_RED, BLACK, BLACK);
         break;
      case yellow:
         Neill_SDL_SetDrawColour(&p->sw, WHITE, WHITE, BLACK);
         break;
      case orange:
         Neill_SDL_SetDrawColour(&p->sw, WHITE, L_RED, BLACK);
         break;
      case black:
         Neill_SDL_SetDrawColour(&p->sw, BLACK, BLACK, BLACK);
         break;
      default:
         Neill_SDL_SetDrawColour(&p->sw, BLACK, BLACK, BLACK);
         break;
   }
   Neill_SDL_RenderDrawCircle(p->sw.renderer, p->turt->x, p->turt->y, r);
   Neill_SDL_RenderFillCircle(p->sw.renderer, p->turt->x, p->turt->y, r);
   Neill_SDL_UpdateScreen(&p->sw);
   Neill_SDL_Events(&p->sw);
}

bool Instruction_ext(Program *p)
{
   if(Do_ext(p)|| Move_ext(p) || Set_interp(p) || ModeSelect(p) || Teleport(p) \
   || Explosion(p) || If_statement(p)){
      return true;
   }
   conditional_err(p, "Expected Valid Instruction or }");
   return false;
}

bool Teleport(Program *p)
{
   if(!strcmp(p->words[p->cw], "TELEPORT")){
      p->turt->x = rand()%WWIDTH;
      p->turt->y = rand()%WHEIGHT;
      if(!p->tflag){
         draw_teleport(p);
      }
      return true;
   }
   return false;
}

bool If_statement(Program *p)
{
   if(!strcmp(p->words[p->cw], "IF")){
      p->cw++;
      if(!strcmp(p->words[p->cw++], "(")){
         if(Condition(p)){
            p->cw++;
            if(!strcmp(p->words[p->cw++], ")")){
               if(!strcmp(p->words[p->cw++], "[")){
                  if(Instrctlst_ext(p)){
                     return true;
                  }
                  conditional_err(p, "Expected ]");
               }
               conditional_err(p, "Expected [");
            }
            conditional_err(p, "Expected )");
         }
         p->cw++;
         update_currentwd(p);
         return true;
      }
      conditional_err(p, "Expected (");
   }
   return false;
}

void update_currentwd(Program *p)
{
   stack* s = stack_init();
   double d = 1;
   if(strcmp(p->words[p->cw++], ")")){
      conditional_err(p, "Expected )");
   }
   if(strcmp(p->words[p->cw], "[")){
      conditional_err(p, "Expected [");
   }
   stack_push(s, d);
   while(count_stack(s) && p->cw <= p->numwords){
      p->cw++;
      if(!strcmp(p->words[p->cw], "[")){
         stack_push(s, d);
      }
      if(!strcmp(p->words[p->cw], "]")){
         stack_pop(s, &d);
      }
   }
   stack_free(s);
   if(!strlen(p->words[p->cw+1])){
      conditional_err(p, "Failure to Balance Brackets");
   }
}

void draw_teleport(Program *p)
{
   double i = 0.01;
   while(i <= 5){
      Neill_SDL_SetDrawColour(&p->sw, BLACK, WHITE, WHITE);
      SDL_Delay(0);
      Neill_SDL_RenderDrawCircle(p->sw.renderer, p->turt->x, p->turt->y, i);
      Neill_SDL_RenderFillCircle(p->sw.renderer, p->turt->x, p->turt->y, i);
      Neill_SDL_UpdateScreen(&p->sw);
      Neill_SDL_Events(&p->sw);
      Neill_SDL_SetDrawColour(&p->sw, BLACK, BLACK, BLACK);
      SDL_Delay(0);
      Neill_SDL_RenderDrawCircle(p->sw.renderer, p->turt->x, p->turt->y, i);
      Neill_SDL_RenderFillCircle(p->sw.renderer, p->turt->x, p->turt->y, i);
      Neill_SDL_UpdateScreen(&p->sw);
      Neill_SDL_Events(&p->sw);
      i += 0.05;
   }
   Neill_SDL_SetDrawColour(&p->sw, BLACK, BLACK, BLACK);
   Neill_SDL_RenderDrawCircle(p->sw.renderer, p->turt->x, p->turt->y, i);
   Neill_SDL_RenderFillCircle(p->sw.renderer, p->turt->x, p->turt->y, i);
}

bool true_mode(Program *p, mode m)
{
   if(!strcmp(p->words[p->cw], "TRUE")){
      if(m == p->turt->m){
         return true;
      }
      return false;
   }
   if(!strcmp(p->words[p->cw], "FALSE")){
      if(m != p->turt->m){
         return true;
      }
      return false;
   }
   else{
      conditional_err(p, "Expected TRUE or FALSE");
      return false;
   }
}

bool true_var(Program *p, char *var)
{
   if(access_vnum(p, p->words[p->cw])){
      if(is_num(p->words[p->cw])){
         if(double_cmp(strtod(p->words[p->cw], NULL), value(p, var))==e){
            return true;
         }
         return false;
      }
      else{
         if(is_var(p->words[p->cw])){
            if(double_cmp(value(p, p->words[p->cw]),value(p, var))==e){
               return true;
            }
            return false;
         }
      }
   }
   conditional_err(p, "Expected Set Variable or Number");
   return false;
}

bool Condition(Program *p)
{
   mode m;
   m = Str_to_mode(p->words[p->cw]);
   if(m != invalid){
      p->cw++;
      if(condition_mode(p, m)){
         return true;
      }
      return false;
   }

   else if(is_var(p->words[p->cw])){
      if(condition_varnum(p)){
         return true;
      }
      return false;
   }
   conditional_err(p, "Expected Mode or Set Variable");
   return false;
}

bool condition_varnum(Program *p)
{
   char *var = p->words[p->cw];
   if(is_set_var(p, var)){
      p->cw++;
      if(!strcmp(p->words[p->cw++], "==")){
         if(true_var(p, var)){
            return true;
         }
         return false;
      }
      conditional_err(p, "Expected ==");
   }
   return false;
}

bool condition_mode(Program *p, mode m)
{
   if(!strcmp(p->words[p->cw++], "==")){
      if(true_mode(p, m)){
         return true;
      }
      return false;
   }
   conditional_err(p, "Expected ==");
   return false;
}

bool Do_ext(Program *p)
{
   char iter_var[2] = {'\0'};
   double current_iter = 0.0, max_loop_int;
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "DO")){
         p->cw++;
         if(is_var(p->words[p->cw])){
            strcpy(iter_var, p->words[p->cw++]);
            if(!strcmp(p->words[p->cw++], "FROM")){
               if(access_vnum(p, p->words[p->cw])){
                  current_iter = value(p, p->words[p->cw++]);
                  set_value(p, iter_var, current_iter);
                     if(!strcmp(p->words[p->cw++], "TO")){
                        if(access_vnum(p, p->words[p->cw])){
                           max_loop_int = value(p, p->words[p->cw++]);
                           if(check_endpoints(current_iter, max_loop_int)){
                              if(!strcmp(p->words[p->cw++], "{")){
                                 if(do_loop_ext(p, max_loop_int, iter_var)){
                                    return true;
                                 }
                              }
                              conditional_err(p, "Expected {");
                           }
                        }
                        conditional_err(p, "Expected a Set Variable or Number");
                     }
                     conditional_err(p, "Expected TO");
                  }
                  conditional_err(p, "Expected a Set Variable or Number");
            }
            conditional_err(p, "Expected FROM");
         }
         conditional_err(p, "Expected a Variable");
      }
   }
   return false;
}

bool do_loop_ext(Program *p, double max, char iter_var[2])
{
   int current_word, final_word = 0;
   current_word = p->cw;
   while(value(p, iter_var) <= max){
      if(!Instrctlst_ext(p)){
         return false;
      }
      final_word = p->cw;
      p->cw = current_word;
      set_value(p, iter_var, value(p, iter_var)+1);
   }
   set_value(p, iter_var, max);
   /* Resetting current word to first instruction after loop */
   p->cw = final_word;
   return true;
}

bool Move_ext(Program *p)
{
   move m = none;
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "FD")){
         m = fd;
      }
      if(!strcmp(p->words[p->cw], "LT")){
         m = lt;
      }
      if(!strcmp(p->words[p->cw], "RT")){
         m = rt;
      }
      if(m != none){
         p->cw++;
         if(valid_movesize_ex(p, m)){
            return true;
         }
      }
   }
   return false;
}

bool valid_movesize_ex(Program *p, move m)
{
   if(access_vnum(p, p->words[p->cw])){
      if(assign_move_ext(p, m)){
         return true;
      }
      return false;
   }
   conditional_err(p, "Expected Valid Variable or Number");
   return false;
}

bool assign_move_ext(Program *p, move m)
{
   double size;
   size = value(p, p->words[p->cw]);
   if(m == fd){
      if(new_pos(p, size)){
         if(!p->tflag){
            draw_line_ext(p);
         }
         return true;
      }
   }
   if(m == lt || m == rt){
      if(new_angle(p, m, size)){
         return true;
      }
      return false;
   }
   return false;
}

void draw_line_ext(Program *p)
{
   if(!p->tflag && p->turt->m != ghost){
      if(p->turt->m == normal){
         Neill_SDL_SetDrawColour(&p->sw, WHITE, WHITE, WHITE);
         SDL_Delay(MILLISECONDDELAY_2);
      }
      if(p->turt->m == fast){
         Neill_SDL_SetDrawColour(&p->sw, WHITE, BLACK, BLACK);
         SDL_Delay(MILLISECONDDELAY_2-15);
      }
      if(p->turt->m == slow){
         Neill_SDL_SetDrawColour(&p->sw, BLACK, WHITE, GREEN);
         SDL_Delay(MILLISECONDDELAY_2+10);
      }
      if(p->turt->m == rainbow){
         rainbow_set(p);
         SDL_Delay(MILLISECONDDELAY_2);
      }
      SDL_RenderDrawLine(p->sw.renderer, p->turt->prev_x, p->turt->prev_y, \
      p->turt->x, p->turt->y);
      Neill_SDL_UpdateScreen(&p->sw);
      Neill_SDL_Events(&p->sw);
   }
}


void rainbow_set(Program *p)
{
   switch(p->turt->rainbow_cnt++){
      case 0:
         /* Red */
         Neill_SDL_SetDrawColour(&p->sw, WHITE, BLACK, BLACK);
         break;
      case 1:
         /* Orange */
         Neill_SDL_SetDrawColour(&p->sw, WHITE, ORANGE, BLACK);
         break;
      case 2:
         /* Yellow */
         Neill_SDL_SetDrawColour(&p->sw, WHITE, WHITE, BLACK);
         break;
      case 3:
         /* Green */
         Neill_SDL_SetDrawColour(&p->sw, BLACK, GREEN, BLACK);
         break;
      case 4:
         /* Blue */
         Neill_SDL_SetDrawColour(&p->sw, BLACK, BLACK, WHITE);
         break;
      case 5:
         /* Violet */
         Neill_SDL_SetDrawColour(&p->sw, VIOLET, BLACK, GREEN);
         break;
      case 6:
         /* Pink */
         Neill_SDL_SetDrawColour(&p->sw, PINK, GREEN, PINK);
         break;
      default:
         break;
   }
   p->turt->rainbow_cnt %= 7;
}

void test(void)
{
   Program* p2 = prog_init(MAXNUMTOKENS);
   Program* p3 = prog_init(MAXNUMTOKENS);
   Program* p4 = prog_init(MAXNUMTOKENS);
   Program* p5 = prog_init(MAXNUMTOKENS);
   Program* p6 = prog_init(MAXNUMTOKENS);
   Program* p7 = prog_init(MAXNUMTOKENS);
   stack *s = stack_init();
   int i, cnt = 0;
   double d1;
   double d2;
   double hypot;
   move m = none;
   mode curr_mode;

   char* fd1 = "FD";
   char* lt1 = "LT";
   char* rt1 = "RT";
   char* cb = "}";
   char* ob = "{";
   char* sc = ";";
   char* set = "SET";
   char* sign = ":=";
   char* do_lbl = "DO";
   char* from = "FROM";
   char* to = "TO";

   char* c1 = "a";
   char* c2 = "A";
   char* c4 = "X";

   char *num1 = "1";
   char *num2 = "10";
   char *num3 = "3002";
   char *num4 = "010";
   char *num8 = "1111wwww";
   char *num9 = "0.5";
   char *num10 = "1.5";
   char *num11 = "99.5";
   char *num12 = "0.314159265359";
   char *num13 = "0.333333333333";
   /*Should we allow this?*/
   char *num15 = ".456";
   char *num16 = "1..456";
   char *num18 = "-2";

   /* Prog_init tests */
   assert(p2);
   assert(p2->s);
   assert(p2->words);
   assert(p2->turt);
   assert(p3);
   assert(p4);
   assert(p4->var_set[0] == false);
   assert(p5);
   assert(p6);
   assert(p7);

   for(i = 0; i < MAXNUMTOKENS; i++){
      p2->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
      p3->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
      p4->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
      p5->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
      p6->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
      p7->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
   }

   p2->tflag = true;
   p3->tflag = true;
   p4->tflag = true;
   p5->tflag = true;
   p6->tflag = true;
   p7->tflag = true;

   p2->numwords = MAXNUMTOKENS;
   p3->numwords = MAXNUMTOKENS;
   p4->numwords = MAXNUMTOKENS;
   p5->numwords = MAXNUMTOKENS;
   p6->numwords = MAXNUMTOKENS;
   p7->numwords = MAXNUMTOKENS;


/*///////////////////////////////////*/
/*///////////////////////////////////*/
/*///////////////////////////////////*/
/*////... Extension tests ...////////*/
/*///////////////////////////////////*/
/*///////////////////////////////////*/
/*///////////////////////////////////*/

   /* eq_mode tests */
   p7->turt->m = fast;
   assert(!eq_mode(p7, ghost));
   assert(eq_mode(p7, fast));
   assert(!eq_mode(p7, slow));
   assert(!eq_mode(p7, rainbow));
   p7->turt->m = normal;
   assert(!eq_mode(p7, ghost));
   assert(!eq_mode(p7, fast));
   assert(!eq_mode(p7, slow));
   assert(!eq_mode(p7, rainbow));

   /* ModeSelect Tests */
   assert(Str_to_mode("FAST") == fast);
   assert(Str_to_mode("FASt") != fast);
   assert(Str_to_mode("SLOW") == slow);
   assert(Str_to_mode("GHOST") == ghost);
   assert(Str_to_mode("RAINBOW") == rainbow);
   assert(Str_to_mode("NORMAL") == normal);
   assert(Str_to_mode("incorrectentry") == invalid);

   p6->cw = 0;
   strcpy(p6->words[p6->cw], "ACTIVATED");
   strcpy(p6->words[p6->cw+1], "GHOST");
   assert(!ModeSelect(p6));
   p6->cw++;

   strcpy(p6->words[p6->cw], "ACTIVATE");
   strcpy(p6->words[p6->cw+1], "GHOST");
   assert(ModeSelect(p6));
   assert(p6->turt->m == ghost);
   p6->cw++;

   p6->turt->m = normal;
   assert(p6->turt->m == normal);

   strcpy(p6->words[p6->cw], "ACTIVATE");
   strcpy(p6->words[p6->cw+1], "FAST");
   assert(ModeSelect(p6));
   assert(p6->turt->m == fast);
   p6->cw++;

   strcpy(p6->words[p6->cw], "ACTIVATE");
   assert(!ModeSelect(p6));

   p6->turt->m = normal;
   assert(p6->turt->m == normal);

   strcpy(p6->words[p6->cw], "ACTIVATE");
   strcpy(p6->words[p6->cw+1], "SLOW");
   assert(ModeSelect(p6));
   assert(p6->turt->m == slow);
   p6->cw++;

   p6->turt->m = normal;
   assert(p6->turt->m == normal);

   strcpy(p6->words[p6->cw], "DEACTIVATE");
   strcpy(p6->words[p6->cw+1], "NORMAL");
   assert(!ModeSelect(p6));
   /* testing still normal after returning false ... */
   assert(p6->turt->m == normal);
   p6->cw++;

   strcpy(p6->words[p6->cw], "ACTIVATE");
   strcpy(p6->words[p6->cw+1], "DEACTIVATE");
   assert(!ModeSelect(p6));
   assert(p6->turt->m == normal);
   p6->cw++;

   strcpy(p6->words[p6->cw], "ACTIVATE");
   strcpy(p6->words[p6->cw+1], "normal");
   assert(!ModeSelect(p6));
   assert(p6->turt->m == normal);
   p6->cw++;

   strcpy(p6->words[p6->cw], "ACTIVATE");
   strcpy(p6->words[p6->cw+1], "NORMAL");
   assert(ModeSelect(p6));
   assert(p6->turt->m == normal);
   p6->cw++;

   strcpy(p6->words[p6->cw], "ACTIVATE");
   strcpy(p6->words[p6->cw+1], "FAST");
   assert(ModeSelect(p6));
   assert(p6->turt->m == fast);
   p6->cw++;

   strcpy(p6->words[p6->cw], "DEACTIVATE");
   strcpy(p6->words[p6->cw+1], "FAST");
   assert(ModeSelect(p6));
   p6->cw++;
   assert(p6->turt->m == normal);

   strcpy(p6->words[p6->cw], "DEACTIVATE");
   strcpy(p6->words[p6->cw+1], "FAST");
   assert(!ModeSelect(p6));
   p6->cw++;

   strcpy(p6->words[p6->cw], "activate");
   strcpy(p6->words[p6->cw+1], "FAST");
   assert(!ModeSelect(p6));
   p6->cw++;

   strcpy(p6->words[p6->cw], "ACTIVATE");
   strcpy(p6->words[p6->cw+1], "FASTER");
   assert(!ModeSelect(p6));
   p6->cw++;

   strcpy(p6->words[p6->cw], "FASTER");
   assert(!ModeSelect(p6));
   p6->cw++;

   strcpy(p6->words[p6->cw], "DEACTIVATE");
   strcpy(p6->words[p6->cw+1], "normal");
   assert(!ModeSelect(p6));
   p6->cw++;

   /* Testing modeselect in Main_ext */
   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], do_lbl);
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], from);
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], to);
   strcpy(p4->words[p4->cw+6], "8");
   strcpy(p4->words[p4->cw+7], ob);
   strcpy(p4->words[p4->cw+8], "ACTIVATE");
   strcpy(p4->words[p4->cw+9], "GHOST");
   strcpy(p4->words[p4->cw+10], fd1);
   strcpy(p4->words[p4->cw+11], "30");
   strcpy(p4->words[p4->cw+12], lt1);
   strcpy(p4->words[p4->cw+13], "45");
   strcpy(p4->words[p4->cw+14], "DEACTIVATE");
   strcpy(p4->words[p4->cw+15], "GHOST");
   strcpy(p4->words[p4->cw+16], cb);
   strcpy(p4->words[p4->cw+17], cb);
   assert(Main_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], do_lbl);
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], from);
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], to);
   strcpy(p4->words[p4->cw+6], "8");
   strcpy(p4->words[p4->cw+7], ob);
   strcpy(p4->words[p4->cw+8], "ACTIVATE");
   strcpy(p4->words[p4->cw+9], "NORMAL");
   strcpy(p4->words[p4->cw+10], cb);
   strcpy(p4->words[p4->cw+11], cb);
   assert(Main_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], do_lbl);
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], from);
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], to);
   strcpy(p4->words[p4->cw+6], "8");
   strcpy(p4->words[p4->cw+7], ob);
   strcpy(p4->words[p4->cw+8], "DEACTIVATE");
   strcpy(p4->words[p4->cw+9], "FAST");
   strcpy(p4->words[p4->cw+10], cb);
   strcpy(p4->words[p4->cw+11], cb);
   assert(!Main_ext(p4));
   p4->cw++;

   /* Testing modeselect and rainbowcnt */
   assert(p6->turt->rainbow_cnt == 0);
   strcpy(p6->words[p6->cw], "ACTIVATE");
   strcpy(p6->words[p6->cw+1], "RAINBOW");
   assert(ModeSelect(p6));
   rainbow_set(p6);
   assert(p6->turt->m == rainbow);
   assert(p6->turt->rainbow_cnt == 1);
   p6->cw++;

   rainbow_set(p6);
   assert(p6->turt->m == rainbow);
   assert(p6->turt->rainbow_cnt == 2);
   p6->cw++;

   rainbow_set(p6);
   assert(p6->turt->m == rainbow);
   assert(p6->turt->rainbow_cnt == 3);
   p6->cw++;

   rainbow_set(p6);
   assert(p6->turt->m == rainbow);
   assert(p6->turt->rainbow_cnt == 4);
   p6->cw++;

   rainbow_set(p6);
   assert(p6->turt->m == rainbow);
   assert(p6->turt->rainbow_cnt == 5);
   p6->cw++;

   rainbow_set(p6);
   assert(p6->turt->m == rainbow);
   assert(p6->turt->rainbow_cnt == 6);
   p6->cw++;

   rainbow_set(p6);
   assert(p6->turt->m == rainbow);
   assert(p6->turt->rainbow_cnt == 0);
   p6->cw++;

   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], do_lbl);
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], from);
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], to);
   strcpy(p4->words[p4->cw+6], "8");
   strcpy(p4->words[p4->cw+7], ob);
   strcpy(p4->words[p4->cw+8], "ACTIVATE");
   strcpy(p4->words[p4->cw+9], "RAINBOW");
   strcpy(p4->words[p4->cw+10], fd1);
   strcpy(p4->words[p4->cw+11], "30");
   strcpy(p4->words[p4->cw+12], lt1);
   strcpy(p4->words[p4->cw+13], "45");
   strcpy(p4->words[p4->cw+14], "DEACTIVATE");
   strcpy(p4->words[p4->cw+15], "RAINBOW");
   strcpy(p4->words[p4->cw+16], cb);
   strcpy(p4->words[p4->cw+17], cb);
   assert(Main_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], do_lbl);
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], from);
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], to);
   strcpy(p4->words[p4->cw+6], "8");
   strcpy(p4->words[p4->cw+7], ob);
   strcpy(p4->words[p4->cw+8], "ACTIVATE");
   strcpy(p4->words[p4->cw+9], "GHOST");
   strcpy(p4->words[p4->cw+10], fd1);
   strcpy(p4->words[p4->cw+11], "30");
   strcpy(p4->words[p4->cw+12], lt1);
   strcpy(p4->words[p4->cw+13], "45");
   strcpy(p4->words[p4->cw+14], "DEACTIVATE");
   strcpy(p4->words[p4->cw+15], "RAINBOW");
   strcpy(p4->words[p4->cw+16], cb);
   strcpy(p4->words[p4->cw+17], cb);
   assert(!Main_ext(p4));
   p4->cw++;
   p4->cw++;

   /* do_loop_ext tests */
   set_value(p7, "A", 5.0);
   strcpy(p7->words[p7->cw], cb);
   assert(do_loop_ext(p7, 10.0, "A"));
   assert(double_cmp(value(p7, "A"), 10.0)==e);
   p7->cw++;

   set_value(p7, "A", 10.0);
   strcpy(p7->words[p7->cw], "FD");
   strcpy(p7->words[p7->cw+1], "-2");
   strcpy(p7->words[p7->cw+2], "LT");
   strcpy(p7->words[p7->cw+3], "RT");
   strcpy(p7->words[p7->cw+4], cb);
   strcpy(p7->words[p7->cw+5], cb);
   assert(!do_loop_ext(p7, 11.0, "A"));
   p7->cw++;

   set_value(p7, "A", 10.0);
   strcpy(p7->words[p7->cw], "FD");
   strcpy(p7->words[p7->cw+1], "-2");
   strcpy(p7->words[p7->cw+2], "LT");
   strcpy(p7->words[p7->cw+3], "20");
   strcpy(p7->words[p7->cw+4], cb);
   assert(do_loop_ext(p7, 11.0, "A"));
   assert(double_cmp(value(p7, "A"), 11.0)==e);
   p7->cw++;

   /* Valid do loop, where start = max, runs 1 iteration */
   set_value(p7, "A", 10.0);
   strcpy(p7->words[p7->cw], "FD");
   strcpy(p7->words[p7->cw+1], "-2");
   strcpy(p7->words[p7->cw+2], "LT");
   strcpy(p7->words[p7->cw+3], "20");
   strcpy(p7->words[p7->cw+4], cb);
   assert(do_loop_ext(p7, 10.0, "A"));
   assert(double_cmp(value(p7, "A"), 10.0)==e);
   p7->cw++;

   set_value(p7, "A", 1.0);
   strcpy(p7->words[p7->cw], "ACTIVATE");
   strcpy(p7->words[p7->cw+1], "FAST");
   strcpy(p7->words[p7->cw+2], "FD");
   strcpy(p7->words[p7->cw+3], "30");
   strcpy(p7->words[p7->cw+4], "DEACTIVATE");
   strcpy(p7->words[p7->cw+5], "FAST");
   strcpy(p7->words[p7->cw+6], cb);
   assert(do_loop_ext(p7, 10.0, "A"));
   assert(double_cmp(value(p7, "A"), 10.0)==e);
   p7->cw++;

   /* Assign_move_ext tests */
   m = none;
   assert(assign_move_ext(p7, lt));
   assert(assign_move_ext(p7, rt));
   assert(assign_move_ext(p7, fd));
   assert(!assign_move_ext(p7, m));

   /* valid_movesize_Ex tests */
   strcpy(p7->words[p7->cw], "A");
   assert(valid_movesize_ex(p7, fd));
   p7->cw++;
   strcpy(p7->words[p7->cw], "A");
   assert(valid_movesize_ex(p7, lt));
   p7->cw++;
   strcpy(p7->words[p7->cw], "A");
   assert(valid_movesize_ex(p7, rt));
   p7->cw++;
   strcpy(p7->words[p7->cw], "A");
   assert(!valid_movesize_ex(p7, none));
   p7->cw++;
   strcpy(p7->words[p7->cw], "N");
   assert(!valid_movesize_ex(p7, fd));
   p7->cw++;
   strcpy(p7->words[p7->cw], "n");
   assert(!valid_movesize_ex(p7, fd));
   p7->cw++;
   strcpy(p7->words[p7->cw], "1202.20110");
   assert(valid_movesize_ex(p7, rt));
   p7->cw++;

   /* Explosion tests */
   strcpy(p4->words[p4->cw], "EXPLOSION");
   assert(Explosion(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "eXPLOSION");
   assert(!Explosion(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "FD");
   strcpy(p4->words[p4->cw+1], "30");
   strcpy(p4->words[p4->cw+2], "LT");
   strcpy(p4->words[p4->cw+3], "30");
   strcpy(p4->words[p4->cw+4], "FD");
   strcpy(p4->words[p4->cw+5], "30");
   strcpy(p4->words[p4->cw+6], "LT");
   strcpy(p4->words[p4->cw+7], "30");
   strcpy(p4->words[p4->cw+8], "EXPLOSION");
   strcpy(p4->words[p4->cw+9], cb);
   assert(Instrctlst_ext(p4));
   p4->cw++;

   p6->cw = 0;
   strcpy(p6->words[p6->cw], ob);
   strcpy(p6->words[p6->cw+1], do_lbl);
   strcpy(p6->words[p6->cw+2], "A");
   strcpy(p6->words[p6->cw+3], from);
   strcpy(p6->words[p6->cw+4], "1");
   strcpy(p6->words[p6->cw+5], to);
   strcpy(p6->words[p6->cw+6], "8");
   strcpy(p6->words[p6->cw+7], ob);
   strcpy(p6->words[p6->cw+8], "ACTIVATE");
   strcpy(p6->words[p6->cw+9], "RAINBOW");
   strcpy(p6->words[p6->cw+10], fd1);
   strcpy(p6->words[p6->cw+11], "30");
   strcpy(p6->words[p6->cw+12], lt1);
   strcpy(p6->words[p6->cw+13], "45");
   strcpy(p6->words[p6->cw+14], "DEACTIVATE");
   strcpy(p6->words[p6->cw+15], "RAINBOW");
   strcpy(p6->words[p6->cw+16], cb);
   strcpy(p6->words[p6->cw+17], "EXPLOSION");
   strcpy(p6->words[p6->cw+18], cb);
   assert(Main_ext(p6));
   p6->cw++;

   strcpy(p6->words[p6->cw], ob);
   strcpy(p6->words[p6->cw+1], do_lbl);
   strcpy(p6->words[p6->cw+2], "A");
   strcpy(p6->words[p6->cw+3], from);
   strcpy(p6->words[p6->cw+4], "1");
   strcpy(p6->words[p6->cw+5], to);
   strcpy(p6->words[p6->cw+6], "8");
   strcpy(p6->words[p6->cw+7], ob);
   strcpy(p6->words[p6->cw+8], "ACTIVATE");
   strcpy(p6->words[p6->cw+9], "RAINBOW");
   strcpy(p6->words[p6->cw+10], fd1);
   strcpy(p6->words[p6->cw+11], "30");
   strcpy(p6->words[p6->cw+12], lt1);
   strcpy(p6->words[p6->cw+14], "45");
   strcpy(p6->words[p6->cw+15], "EXPLOSION");
   strcpy(p6->words[p6->cw+16], cb);
   strcpy(p6->words[p6->cw+17], "45");
   strcpy(p6->words[p6->cw+18], cb);
   assert(!Main_ext(p6));
   p6->cw++;

   /* teleport tests */
   strcpy(p4->words[p4->cw], "TELEPORT");
   assert(Teleport(p4));
   p4->cw++;

   /* Testing coordinates change 90% of the time. */
   strcpy(p4->words[p4->cw], "TELEPORT");
   for(i = 0; i < 100; i++){
      d1 = p4->turt->x;
      d2 = p4->turt->y;
      assert(Teleport(p4));
      if(!double_cmp(p4->turt->x, d1) && !double_cmp(p4->turt->y, d2)){
         cnt++;
      }
   }
   assert(cnt <= 10);

   strcpy(p4->words[p4->cw], "TELEPORT");
   assert(Instruction_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "TELEPORT");
   strcpy(p4->words[p4->cw+1], "FD");
   strcpy(p4->words[p4->cw+2], "30");
   strcpy(p4->words[p4->cw+3], "LT");
   strcpy(p4->words[p4->cw+4], "60");
   strcpy(p4->words[p4->cw+5], "EXPLOSION");
   strcpy(p4->words[p4->cw+6], "}");
   assert(Instrctlst_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], do_lbl);
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], from);
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], to);
   strcpy(p4->words[p4->cw+6], "8");
   strcpy(p4->words[p4->cw+7], ob);
   strcpy(p4->words[p4->cw+8], "ACTIVATE");
   strcpy(p4->words[p4->cw+9], "RAINBOW");
   strcpy(p4->words[p4->cw+10], fd1);
   strcpy(p4->words[p4->cw+11], "30");
   strcpy(p4->words[p4->cw+12], lt1);
   strcpy(p4->words[p4->cw+13], "45");
   strcpy(p4->words[p4->cw+14], "TELEPORT");
   strcpy(p4->words[p4->cw+15], "DEACTIVATE");
   strcpy(p4->words[p4->cw+16], "RAINBOW");
   strcpy(p4->words[p4->cw+17], cb);
   strcpy(p4->words[p4->cw+18], "EXPLOSION");
   strcpy(p4->words[p4->cw+19], cb);
   assert(Main_ext(p4));
   p4->cw++;

   /* true_mode tests */
   curr_mode = ghost;
   p4->turt->m = ghost;
   strcpy(p4->words[p4->cw], "TRUE");
   assert(true_mode(p4, curr_mode));
   p4->cw++;
   strcpy(p4->words[p4->cw], "DEACTIVATE");
   strcpy(p4->words[p4->cw+1], "GHOST");
   assert(ModeSelect(p4));
   p4->cw++;
   strcpy(p4->words[p4->cw], "TRUE");
   assert(!true_mode(p4, curr_mode));
   p4->cw++;
   strcpy(p4->words[p4->cw], "FALSE");
   assert(true_mode(p4, curr_mode));

   curr_mode = fast;
   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "TRUE");
   assert(true_mode(p4, curr_mode));
   p4->cw++;
   strcpy(p4->words[p4->cw], "DEACTIVATE");
   strcpy(p4->words[p4->cw+1], "GHOST");
   assert(!ModeSelect(p4));
   p4->cw++;
   strcpy(p4->words[p4->cw], "DEACTIVATE");
   strcpy(p4->words[p4->cw+1], "FAST");
   assert(ModeSelect(p4));
   p4->cw++;
   strcpy(p4->words[p4->cw], "TRUE");
   assert(!true_mode(p4, curr_mode));
   p4->cw++;
   strcpy(p4->words[p4->cw], "FALSE");
   assert(true_mode(p4, curr_mode));
   p4->cw++;
   strcpy(p4->words[p4->cw], "false!");
   assert(!true_mode(p4, curr_mode));

   curr_mode = rainbow;
   p4->turt->m = rainbow;
   strcpy(p4->words[p4->cw], "TRUE");
   assert(true_mode(p4, curr_mode));
   p4->cw++;
   strcpy(p4->words[p4->cw], "DEACTIVATE");
   strcpy(p4->words[p4->cw+1], "RAINBOW");
   assert(ModeSelect(p4));
   p4->cw++;
   strcpy(p4->words[p4->cw], "TRUE");
   assert(!true_mode(p4, curr_mode));
   p4->cw++;
   strcpy(p4->words[p4->cw], "FALSE");
   assert(true_mode(p4, curr_mode));
   p4->cw++;
   strcpy(p4->words[p4->cw], "false!");
   assert(!true_mode(p4, curr_mode));

   /* true_var tests */
   p7->var_set[0] = false;
   strcpy(p7->words[p7->cw], "A");
   assert(!true_var(p7, "101"));
   p7->cw++;
   p7->var_set[0] = true;
   p7->vars[0] = 10.0;
   strcpy(p7->words[p7->cw], "A");
   assert(!true_var(p7, "101"));
   p7->cw++;
   p7->vars[0] = 101.0;
   strcpy(p7->words[p7->cw], "A");
   assert(true_var(p7, "101"));
   p7->cw++;
   p7->vars[0] = -102.0212;
   strcpy(p7->words[p7->cw], "A");
   assert(true_var(p7, "-102.0212"));
   p7->cw++;
   p7->vars[1] = -102.0212;
   strcpy(p7->words[p7->cw], "A");
   assert(true_var(p7, "B"));
   p7->cw++;
   p7->var_set[3] = true;
   strcpy(p7->words[p7->cw], "A");
   assert(!true_var(p7, "D"));
   p7->cw++;

   /* condition_mode tests */
   curr_mode = fast;
   p7->turt->m = fast;
   strcpy(p7->words[p7->cw], "==");
   strcpy(p7->words[p7->cw+1], "TRUE");
   assert(condition_mode(p7, curr_mode));
   p7->cw++;

   curr_mode = fast;
   p7->turt->m = fast;
   strcpy(p7->words[p7->cw], "TRUE");
   assert(!condition_mode(p7, curr_mode));
   p7->cw++;

   /* condition_varnum tests */
   p7->var_set[0] = false;
   strcpy(p7->words[p7->cw], "A");
   strcpy(p7->words[p7->cw+1], "==");
   assert(!condition_varnum(p7));
   p7->cw++;

   p7->var_set[0] = true;
   p7->vars[0] = -100.0;
   strcpy(p7->words[p7->cw], "A");
   strcpy(p7->words[p7->cw+1], "==");
   strcpy(p7->words[p7->cw+2], "-100.0");
   assert(condition_varnum(p7));
   p7->cw++;

   p7->var_set[0] = true;
   p7->vars[0] = -100.0;
   strcpy(p7->words[p7->cw], "A");
   strcpy(p7->words[p7->cw+1], "==");
   strcpy(p7->words[p7->cw+2], "-99.999");
   assert(!condition_varnum(p7));
   p7->cw++;

   p7->var_set[0] = true;
   p7->vars[0] = -100.0;
   strcpy(p7->words[p7->cw], "A");
   strcpy(p7->words[p7->cw+1], "-100.0");
   assert(!condition_varnum(p7));
   p7->cw++;

   /* Condition Valid Tests */
   strcpy(p4->words[p4->cw], "INVALID");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "D");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "I");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "9");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "c");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "FAST");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "EXPLOSION");
   assert(!Condition(p4));
   p4->cw++;

   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "FAST");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "TRUE");
   assert(Condition(p4));
   p4->cw++;

   p4->turt->m = normal;
   strcpy(p4->words[p4->cw], "FAST");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "FALSE");
   assert(Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "fast");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "FALSE");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "FAST");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "BOOL");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "FAST");
   strcpy(p4->words[p4->cw+1], "TRUE");
   assert(!Condition(p4));
   p4->cw++;

   p4->vars[3] = 201.019;
   p4->var_set[3] = true;
   p4->cw++;
   strcpy(p4->words[p4->cw], "D");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "201.019");
   assert(Condition(p4));
   p4->cw++;

   p4->vars[0] = 201.019;
   strcpy(p4->words[p4->cw], "D");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "A");
   assert(Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "D");
   strcpy(p4->words[p4->cw+1], "201.019");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "D");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "FALSE");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "D");
   strcpy(p4->words[p4->cw+1], "==");
   assert(!Condition(p4));
   p4->cw++;

   p4->turt->m = normal;
   strcpy(p4->words[p4->cw], "FAST");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "TRUE");
   assert(!Condition(p4));
   p4->cw++;

   p4->turt->m = normal;
   strcpy(p4->words[p4->cw], "GHOST");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "FALSE");
   assert(Condition(p4));
   p4->cw++;

   p4->vars[0] = 0;
   p4->vars[3] = 0;
   strcpy(p4->words[p4->cw], "D");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "201.019");
   assert(!Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "D");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "A");
   assert(Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "D");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "0.0");
   assert(Condition(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "0.0");
   strcpy(p4->words[p4->cw+1], "==");
   strcpy(p4->words[p4->cw+2], "0.0");
   assert(!Condition(p4));
   p4->cw++;

   p4->turt->m = fast;
   p4->cw = 0;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "FAST");
   strcpy(p4->words[p4->cw+3], "==");
   strcpy(p4->words[p4->cw+4], "TRUE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "}");
   strcpy(p4->words[p4->cw+8], "]");
   assert(If_statement(p4));
   p4->cw++;

   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "GHOST");
   strcpy(p4->words[p4->cw+3], "==");
   strcpy(p4->words[p4->cw+4], "TRUE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "]");
   assert(If_statement(p4));
   p4->cw++;

   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "FAST");
   strcpy(p4->words[p4->cw+3], "==");
   strcpy(p4->words[p4->cw+4], "TRUE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+9], "LT");
   strcpy(p4->words[p4->cw+10], "30");
   strcpy(p4->words[p4->cw+11], "]");
   assert(If_statement(p4));
   p4->cw++;

   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "FAST");
   strcpy(p4->words[p4->cw+3], "==");
   strcpy(p4->words[p4->cw+4], "TRUE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+9], "IF");
   strcpy(p4->words[p4->cw+10], "(");
   strcpy(p4->words[p4->cw+11], "FAST");
   strcpy(p4->words[p4->cw+12], "==");
   strcpy(p4->words[p4->cw+13], "TRUE");
   strcpy(p4->words[p4->cw+14], ")");
   strcpy(p4->words[p4->cw+15], "[");
   strcpy(p4->words[p4->cw+16], "FD");
   strcpy(p4->words[p4->cw+17], "30");
   strcpy(p4->words[p4->cw+18], "LT");
   strcpy(p4->words[p4->cw+19], "30");
   strcpy(p4->words[p4->cw+20], "]");
   strcpy(p4->words[p4->cw+21], "]");
   assert(If_statement(p4));
   p4->cw++;

   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "FAST");
   strcpy(p4->words[p4->cw+3], "=");
   strcpy(p4->words[p4->cw+4], "TRUE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+9], "IF");
   strcpy(p4->words[p4->cw+10], "(");
   strcpy(p4->words[p4->cw+11], "FAST");
   strcpy(p4->words[p4->cw+12], "==");
   strcpy(p4->words[p4->cw+13], "TRUE");
   strcpy(p4->words[p4->cw+14], ")");
   strcpy(p4->words[p4->cw+15], "[");
   strcpy(p4->words[p4->cw+16], "FD");
   strcpy(p4->words[p4->cw+17], "30");
   strcpy(p4->words[p4->cw+18], "LT");
   strcpy(p4->words[p4->cw+19], "30");
   strcpy(p4->words[p4->cw+20], "]");
   strcpy(p4->words[p4->cw+21], "]");
   assert(If_statement(p4));
   p4->cw++;

   p4->turt->m = normal;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "FAST");
   strcpy(p4->words[p4->cw+3], "=");
   strcpy(p4->words[p4->cw+4], "TRUE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+9], "IF");
   strcpy(p4->words[p4->cw+10], "(");
   strcpy(p4->words[p4->cw+11], "FAST");
   strcpy(p4->words[p4->cw+12], "==");
   strcpy(p4->words[p4->cw+13], "TRUE");
   strcpy(p4->words[p4->cw+14], ")");
   strcpy(p4->words[p4->cw+15], "[");
   strcpy(p4->words[p4->cw+16], "FD");
   strcpy(p4->words[p4->cw+17], "30");
   strcpy(p4->words[p4->cw+18], "LT");
   strcpy(p4->words[p4->cw+19], "30");
   strcpy(p4->words[p4->cw+20], "]");
   strcpy(p4->words[p4->cw+21], "]");
   assert(If_statement(p4));
   p4->cw++;

   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "FAST");
   strcpy(p4->words[p4->cw+3], "==");
   strcpy(p4->words[p4->cw+4], "TRUE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+9], "IF");
   strcpy(p4->words[p4->cw+10], "(");
   strcpy(p4->words[p4->cw+11], "FAST");
   strcpy(p4->words[p4->cw+12], "==");
   strcpy(p4->words[p4->cw+13], "TRUE");
   strcpy(p4->words[p4->cw+14], ")");
   strcpy(p4->words[p4->cw+15], "[");
   strcpy(p4->words[p4->cw+16], "DEACTIVATE");
   strcpy(p4->words[p4->cw+17], "FAST");
   strcpy(p4->words[p4->cw+18], "LT");
   strcpy(p4->words[p4->cw+19], "30");
   strcpy(p4->words[p4->cw+20], "]");
   strcpy(p4->words[p4->cw+21], "]");
   assert(If_statement(p4));
   p4->cw++;

   p4->turt->m = fast;
   p4->var_set[0] = true;
   p4->vars[0] = -2.301;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "FAST");
   strcpy(p4->words[p4->cw+3], "==");
   strcpy(p4->words[p4->cw+4], "TRUE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+9], "IF");
   strcpy(p4->words[p4->cw+10], "(");
   strcpy(p4->words[p4->cw+11], "A");
   strcpy(p4->words[p4->cw+12], "==");
   strcpy(p4->words[p4->cw+13], "-2.301");
   strcpy(p4->words[p4->cw+14], ")");
   strcpy(p4->words[p4->cw+15], "[");
   strcpy(p4->words[p4->cw+16], "DEACTIVATE");
   strcpy(p4->words[p4->cw+17], "FAST");
   strcpy(p4->words[p4->cw+18], "LT");
   strcpy(p4->words[p4->cw+19], "30");
   strcpy(p4->words[p4->cw+20], "]");
   strcpy(p4->words[p4->cw+21], "]");
   assert(If_statement(p4));
   p4->cw++;

   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "If");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "FAST");
   strcpy(p4->words[p4->cw+3], "==");
   strcpy(p4->words[p4->cw+4], "TRUE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+9], "IF");
   strcpy(p4->words[p4->cw+10], "(");
   strcpy(p4->words[p4->cw+11], "FAST");
   strcpy(p4->words[p4->cw+12], "==");
   strcpy(p4->words[p4->cw+13], "TRUE");
   strcpy(p4->words[p4->cw+14], ")");
   strcpy(p4->words[p4->cw+15], "[");
   strcpy(p4->words[p4->cw+16], "FD");
   strcpy(p4->words[p4->cw+17], "30");
   strcpy(p4->words[p4->cw+18], "LT");
   strcpy(p4->words[p4->cw+19], "30");
   strcpy(p4->words[p4->cw+20], "]");
   strcpy(p4->words[p4->cw+21], "]");
   assert(!If_statement(p4));
   p4->cw++;

   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "{");
   strcpy(p4->words[p4->cw+1], "IF");
   strcpy(p4->words[p4->cw+2], "(");
   strcpy(p4->words[p4->cw+3], "FAST");
   strcpy(p4->words[p4->cw+4], "==");
   strcpy(p4->words[p4->cw+5], "TRUE");
   strcpy(p4->words[p4->cw+6], ")");
   strcpy(p4->words[p4->cw+7], "[");
   strcpy(p4->words[p4->cw+8], "FD");
   strcpy(p4->words[p4->cw+9], "30");
   strcpy(p4->words[p4->cw+10], "IF");
   strcpy(p4->words[p4->cw+11], "(");
   strcpy(p4->words[p4->cw+12], "FAST");
   strcpy(p4->words[p4->cw+13], "==");
   strcpy(p4->words[p4->cw+14], "TRUE");
   strcpy(p4->words[p4->cw+15], ")");
   strcpy(p4->words[p4->cw+16], "[");
   strcpy(p4->words[p4->cw+17], "FD");
   strcpy(p4->words[p4->cw+18], "30");
   strcpy(p4->words[p4->cw+19], "LT");
   strcpy(p4->words[p4->cw+20], "30");
   strcpy(p4->words[p4->cw+21], "]");
   strcpy(p4->words[p4->cw+22], "]");
   strcpy(p4->words[p4->cw+23], "}");
   assert(Main_ext(p4));
   p4->cw++;

   /* Update current word function tests */
   p4->cw = 0;
   p4->turt->m = fast;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "FAST");
   strcpy(p4->words[p4->cw+3], "==");
   strcpy(p4->words[p4->cw+4], "FALSE");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+9], "]");
   assert(If_statement(p4));
   assert(!double_cmp(p4->cw, 9.0));

   /* Updating current word if condition does not hold */
   p4->cw = 0;
   strcpy(p4->words[p4->cw], ")");
   strcpy(p4->words[p4->cw+1], "[");
   strcpy(p4->words[p4->cw+2], "FD");
   strcpy(p4->words[p4->cw+3], "30");
   strcpy(p4->words[p4->cw+4], "]");
   update_currentwd(p4);
   assert(double_cmp(p4->cw, 4)==e);

   p4->cw = 0;
   strcpy(p4->words[p4->cw], ")");
   strcpy(p4->words[p4->cw+1], "[");
   strcpy(p4->words[p4->cw+2], "FD");
   strcpy(p4->words[p4->cw+3], "DO");
   strcpy(p4->words[p4->cw+4], "A");
   strcpy(p4->words[p4->cw+5], "FROM");
   strcpy(p4->words[p4->cw+6], "1");
   strcpy(p4->words[p4->cw+7], "TO");
   strcpy(p4->words[p4->cw+8], "100");
   strcpy(p4->words[p4->cw+9], "[");
   strcpy(p4->words[p4->cw+10], "FD");
   strcpy(p4->words[p4->cw+11], "30");
   strcpy(p4->words[p4->cw+12], "]");
   strcpy(p4->words[p4->cw+13], "FD");
   strcpy(p4->words[p4->cw+14], "30");
   strcpy(p4->words[p4->cw+15], "]");
   update_currentwd(p4);
   assert(double_cmp(p4->cw, 15)==e);

   p4->cw = 0;
   strcpy(p4->words[p4->cw], ")");
   strcpy(p4->words[p4->cw+1], "[");
   strcpy(p4->words[p4->cw+2], "[");
   strcpy(p4->words[p4->cw+3], "[");
   strcpy(p4->words[p4->cw+4], "[");
   strcpy(p4->words[p4->cw+5], "[");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "[");
   strcpy(p4->words[p4->cw+8], "[");
   strcpy(p4->words[p4->cw+9], "[");
   strcpy(p4->words[p4->cw+10], "]");
   strcpy(p4->words[p4->cw+11], "]");
   strcpy(p4->words[p4->cw+12], "]");
   strcpy(p4->words[p4->cw+13], "]");
   strcpy(p4->words[p4->cw+14], "]");
   strcpy(p4->words[p4->cw+15], "]");
   strcpy(p4->words[p4->cw+16], "]");
   strcpy(p4->words[p4->cw+17], "]");
   strcpy(p4->words[p4->cw+18], "]"); /* Balanced Here */
   strcpy(p4->words[p4->cw+19], "]");
   strcpy(p4->words[p4->cw+20], "]");
   strcpy(p4->words[p4->cw+21], "]");
   update_currentwd(p4);
   assert(double_cmp(p4->cw, 18)==e);


/* Here do a nested if statement, where the
interior if statement would evaluate to be true */
   p4->cw = 0;
   p4->vars[0] = 0;
   strcpy(p4->words[p4->cw], "IF");
   strcpy(p4->words[p4->cw+1], "(");
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], "==");
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], ")");
   strcpy(p4->words[p4->cw+6], "[");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+7], "FD");
   strcpy(p4->words[p4->cw+8], "30");
   strcpy(p4->words[p4->cw+9], "IF");
   strcpy(p4->words[p4->cw+10], "(");
   strcpy(p4->words[p4->cw+11], "A");
   strcpy(p4->words[p4->cw+12], "==");
   strcpy(p4->words[p4->cw+13], "0");
   strcpy(p4->words[p4->cw+14], ")");
   strcpy(p4->words[p4->cw+15], "[");
   strcpy(p4->words[p4->cw+16], "SET");
   strcpy(p4->words[p4->cw+17], "A");
   strcpy(p4->words[p4->cw+18], ":=");
   strcpy(p4->words[p4->cw+19], "100");
   strcpy(p4->words[p4->cw+20], "]");
   strcpy(p4->words[p4->cw+21], "FD");
   strcpy(p4->words[p4->cw+22], "30");
   strcpy(p4->words[p4->cw+23], "FD");
   strcpy(p4->words[p4->cw+24], "30");
   strcpy(p4->words[p4->cw+25], "]");
   assert(If_statement(p4));
   assert(!double_cmp(p4->cw, 25.0));
   assert(!double_cmp(p4->vars[0], 0.0));


/*//////////////////////////////////////////*/
 /* PREVIOUS TESTS - TESTING TO ENSURE
    INSTRUCTIONS THAT PREVIOUSLY PASSED
    INTERPRETATION + PARSING STILL PASS */
/*//////////////////////////////////////////*/

   /*Testing the Instrctlst_ext function*/
   strcpy(p2->words[p2->cw], cb);
   assert(Instrctlst_ext(p2));

   strcpy(p2->words[p2->cw], "}");
   assert(Instrctlst_ext(p2));

   strcpy(p2->words[p2->cw], ")");
   assert(!Instrctlst_ext(p2));

   strcpy(p2->words[p2->cw], fd1);
   strcpy(p2->words[p2->cw+1], num18);
   strcpy(p2->words[p2->cw+2], cb);
   assert(Instrctlst_ext(p2));

   strcpy(p2->words[p2->cw], fd1);
   strcpy(p2->words[p2->cw+1], num18);
   assert(!Instrctlst_ext(p2));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num18);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num18);
   assert(!Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], cb);
   assert(!Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   assert(!Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num3);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num8);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num10);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num9);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num12);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num13);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num2);
   strcpy(p3->words[p3->cw+2], lt1);
   strcpy(p3->words[p3->cw+3], num9);
   strcpy(p3->words[p3->cw+4], rt1);
   strcpy(p3->words[p3->cw+5], num10);
   strcpy(p3->words[p3->cw+6], cb);
   assert(Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num2);
   strcpy(p3->words[p3->cw+2], lt1);
   strcpy(p3->words[p3->cw+3], num9);
   strcpy(p3->words[p3->cw+4], lt1);
   strcpy(p3->words[p3->cw+5], num10);
   strcpy(p3->words[p3->cw+6], lt1);
   strcpy(p3->words[p3->cw+7], num10);
   strcpy(p3->words[p3->cw+8], cb);
   assert(Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num2);
   strcpy(p3->words[p3->cw+2], num15);
   strcpy(p3->words[p3->cw+3], cb);
   assert(!Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], rt1);
   strcpy(p3->words[p3->cw+1], num16);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Instrctlst_ext(p3));

   strcpy(p3->words[p3->cw], rt1);
   strcpy(p3->words[p3->cw+1], num16);
   assert(!Instrctlst_ext(p3));

   /*Testing the Main_ext function*/
   strcpy(p3->words[p3->cw], ob);
   assert(!Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num16);
   strcpy(p3->words[p3->cw+3], cb);
   assert(!Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num3);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num4);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num11);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_ext(p3));

   strcpy(p3->words[p3->cw], rt1);
   strcpy(p3->words[p3->cw+1], num13);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], rt1);
   strcpy(p3->words[p3->cw+3], num13);
   strcpy(p3->words[p3->cw+4], cb);
   assert(!Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt1);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], fd1);
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt1);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], "fd1!");
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(!Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt1);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], "fd");
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(!Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt1);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], "Fd");
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(!Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   assert(!Main_ext(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], cb);
   assert(Main_ext(p3));

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c4);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], "+");
   strcpy(p4->words[p4->cw+4], "*");
   strcpy(p4->words[p4->cw+5], "/");
   strcpy(p4->words[p4->cw+6], num1);
   strcpy(p4->words[p4->cw+7], c2);
   strcpy(p4->words[p4->cw+8], c4);
   strcpy(p4->words[p4->cw+9], num13);
   strcpy(p4->words[p4->cw+10], num18);
   strcpy(p4->words[p4->cw+11], sc);
   assert(!Instruction_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c4);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], "+");
   strcpy(p4->words[p4->cw+4], "*");
   strcpy(p4->words[p4->cw+5], "/");
   strcpy(p4->words[p4->cw+6], num1);
   strcpy(p4->words[p4->cw+7], c2);
   strcpy(p4->words[p4->cw+8], c4);
   strcpy(p4->words[p4->cw+9], num13);
   strcpy(p4->words[p4->cw+10], num18);
   strcpy(p4->words[p4->cw+11], sc);
   strcpy(p4->words[p4->cw+12], cb);
   assert(!Instrctlst_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c4);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], num1);
   strcpy(p4->words[p4->cw+5], "+");
   strcpy(p4->words[p4->cw+6], sc);
   strcpy(p4->words[p4->cw+7], cb);
   assert(Instrctlst_ext(p4));
   p4->cw++;

   strcpy(p7->words[p7->cw], set);
   strcpy(p7->words[p7->cw+1], c4);
   strcpy(p7->words[p7->cw+2], sign);
   strcpy(p7->words[p7->cw+3], num1);
   strcpy(p7->words[p7->cw+4], num1);
   strcpy(p7->words[p7->cw+5], "+");
   strcpy(p7->words[p7->cw+6], sc);
   assert(!Instrctlst_ext(p7));
   p7->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd1);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], rt1);
   strcpy(p4->words[p4->cw+10], num10);
   strcpy(p4->words[p4->cw+11], rt1);
   strcpy(p4->words[p4->cw+12], num13);
   strcpy(p4->words[p4->cw+13], lt1);
   strcpy(p4->words[p4->cw+14], num11);
   strcpy(p4->words[p4->cw+15], cb);
   assert(!Instruction_ext(p4));
   p4->cw++;

   /*
   ///////////////////////////////////////
   ///////// INTERPRETER TESTS ///////////
   ///////////////////////////////////////
   */


   /* Can't loop from a number bigger than
   end number */

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd1);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], rt1);
   strcpy(p4->words[p4->cw+10], num10);
   strcpy(p4->words[p4->cw+11], rt1);
   strcpy(p4->words[p4->cw+12], num13);
   strcpy(p4->words[p4->cw+13], lt1);
   strcpy(p4->words[p4->cw+14], num11);
   strcpy(p4->words[p4->cw+15], cb);
   assert(!Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num3);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd1);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], rt1);
   strcpy(p4->words[p4->cw+10], num10);
   strcpy(p4->words[p4->cw+11], rt1);
   strcpy(p4->words[p4->cw+12], num13);
   strcpy(p4->words[p4->cw+13], lt1);
   strcpy(p4->words[p4->cw+14], num11);
   strcpy(p4->words[p4->cw+15], cb);
   assert(Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], cb);
   assert(!Do_ext(p4));
   p4->cw++;

   /* If from and to are equal, still do it
   since were doing from and to inclusive */
   strcpy(p4->words[p4->cw], c2);
   strcpy(p4->words[p4->cw+1], from);
   strcpy(p4->words[p4->cw+2], num3);
   strcpy(p4->words[p4->cw+3], to);
   strcpy(p4->words[p4->cw+4], num1);
   strcpy(p4->words[p4->cw+5], ob);
   strcpy(p4->words[p4->cw+6], cb);
   assert(!Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num3);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd1);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], cb);
   assert(Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], from);
   strcpy(p4->words[p4->cw+2], num3);
   strcpy(p4->words[p4->cw+3], to);
   strcpy(p4->words[p4->cw+4], num1);
   strcpy(p4->words[p4->cw+5], ob);
   strcpy(p4->words[p4->cw+6], cb);
   assert(!Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], to);
   strcpy(p4->words[p4->cw+4], num3);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], cb);
   assert(!Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c1);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd1);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], cb);
   assert(!Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], do_lbl);
   strcpy(p4->words[p4->cw+8], cb);
   assert(!Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd1);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], rt1);
   strcpy(p4->words[p4->cw+10], num10);
   strcpy(p4->words[p4->cw+11], rt1);
   strcpy(p4->words[p4->cw+12], num13);
   strcpy(p4->words[p4->cw+13], lt1);
   strcpy(p4->words[p4->cw+14], num11);
   assert(!Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num9);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num10);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], cb);
   assert(Do_ext(p4));
   p4->cw++;


   /* Added loop counters */
   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num3);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], cb);
   assert(Do_ext(p4));
   p4->cw++;

   p4->cw = 0;
   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num3);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd1);
   strcpy(p4->words[p4->cw+8], "30");
   assert(!Do_ext(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num3);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd1);
   strcpy(p4->words[p4->cw+8], "30");
   assert(!Do_ext(p4));
   p4->cw++;

   /* Testing incrementing correctly */
   /* B will be one more, Do A from 1 to 8, 8 included...  */
   p5->vars[1] = 0;
   strcpy(p5->words[p5->cw], do_lbl);
   strcpy(p5->words[p5->cw+1], "B");
   strcpy(p5->words[p5->cw+2], from);
   strcpy(p5->words[p5->cw+3], "1");
   strcpy(p5->words[p5->cw+4], to);
   strcpy(p5->words[p5->cw+5], "10");
   strcpy(p5->words[p5->cw+6], ob);
   strcpy(p5->words[p5->cw+7], cb);
   assert(Do_ext(p5));
   d1 = value(p5, "B");
   assert(double_cmp(d1, 10.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], do_lbl);
   strcpy(p5->words[p5->cw+1], "B");
   strcpy(p5->words[p5->cw+2], from);
   strcpy(p5->words[p5->cw+3], "1.5");
   strcpy(p5->words[p5->cw+4], to);
   strcpy(p5->words[p5->cw+5], "10.5");
   strcpy(p5->words[p5->cw+6], ob);
   strcpy(p5->words[p5->cw+7], cb);
   assert(Do_ext(p5));
   d1 = value(p5, "B");
   assert(double_cmp(d1, 10.5)==e);
   p5->cw++;

   set_value(p5, "D", 1.0);
   set_value(p5, "Z", 1.0);
   strcpy(p5->words[p5->cw], do_lbl);
   strcpy(p5->words[p5->cw+1], "B");
   strcpy(p5->words[p5->cw+2], from);
   strcpy(p5->words[p5->cw+3], "-1");
   strcpy(p5->words[p5->cw+4], to);
   strcpy(p5->words[p5->cw+5], "99");
   strcpy(p5->words[p5->cw+6], ob);
   strcpy(p5->words[p5->cw+7], "SET");
   strcpy(p5->words[p5->cw+8], "A");
   strcpy(p5->words[p5->cw+9], sign);
   strcpy(p5->words[p5->cw+10], "D");
   strcpy(p5->words[p5->cw+11], "2");
   strcpy(p5->words[p5->cw+12], "10");
   strcpy(p5->words[p5->cw+13], "*");
   strcpy(p5->words[p5->cw+14], "+");
   strcpy(p5->words[p5->cw+15], "Z");
   strcpy(p5->words[p5->cw+16], "Z");
   strcpy(p5->words[p5->cw+17], "+");
   strcpy(p5->words[p5->cw+18], "3");
   strcpy(p5->words[p5->cw+19], "-");
   strcpy(p5->words[p5->cw+20], "-");
   strcpy(p5->words[p5->cw+21], ";");
   strcpy(p5->words[p5->cw+22], cb);
   assert(Do_ext(p5));
   d1 = value(p5, "B");
   assert(double_cmp(d1, 99)==e);
   p5->cw++;

   set_value(p5, "A", 0.0);
   assert(is_set_var(p5, "A"));
   set_value(p5, "C", 0.0);
   strcpy(p5->words[p5->cw], do_lbl);
   strcpy(p5->words[p5->cw+1], "A");
   strcpy(p5->words[p5->cw+2], from);
   strcpy(p5->words[p5->cw+3], "1");
   strcpy(p5->words[p5->cw+4], to);
   strcpy(p5->words[p5->cw+5], "100");
   strcpy(p5->words[p5->cw+6], ob);
   strcpy(p5->words[p5->cw+7], set);
   strcpy(p5->words[p5->cw+8], "C");
   strcpy(p5->words[p5->cw+9], ":=");
   strcpy(p5->words[p5->cw+10], "A");
   strcpy(p5->words[p5->cw+11], "1.5");
   strcpy(p5->words[p5->cw+12], "*");
   strcpy(p5->words[p5->cw+13], sc);
   strcpy(p5->words[p5->cw+14], fd1);
   strcpy(p5->words[p5->cw+15], "C");
   strcpy(p5->words[p5->cw+16], rt1);
   strcpy(p5->words[p5->cw+17], "62");
   strcpy(p5->words[p5->cw+18], cb);
   assert(Do_ext(p5));
   d1 = value(p5, "A");
   assert(double_cmp(d1, 100)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], do_lbl);
   strcpy(p5->words[p5->cw+1], "A");
   strcpy(p5->words[p5->cw+2], from);
   strcpy(p5->words[p5->cw+3], "1");
   strcpy(p5->words[p5->cw+4], to);
   strcpy(p5->words[p5->cw+5], "50");
   strcpy(p5->words[p5->cw+6], ob);
   strcpy(p5->words[p5->cw+7], fd1);
   strcpy(p5->words[p5->cw+8], "A");
   strcpy(p5->words[p5->cw+9], rt1);
   strcpy(p5->words[p5->cw+10], "30");
   strcpy(p5->words[p5->cw+11], do_lbl);
   strcpy(p5->words[p5->cw+12], "B");
   strcpy(p5->words[p5->cw+13], from);
   strcpy(p5->words[p5->cw+14], "1");
   strcpy(p5->words[p5->cw+15], to);
   strcpy(p5->words[p5->cw+16], "8");
   strcpy(p5->words[p5->cw+17], ob);
   strcpy(p5->words[p5->cw+18], set);
   strcpy(p5->words[p5->cw+19], "C");
   strcpy(p5->words[p5->cw+20], ":=");
   strcpy(p5->words[p5->cw+21], "A");
   strcpy(p5->words[p5->cw+22], "5");
   strcpy(p5->words[p5->cw+23], "/");
   strcpy(p5->words[p5->cw+24], sc);
   strcpy(p5->words[p5->cw+25], fd1);
   strcpy(p5->words[p5->cw+26], "C");
   strcpy(p5->words[p5->cw+27], "RT");
   strcpy(p5->words[p5->cw+28], "45");
   strcpy(p5->words[p5->cw+29], cb);
   strcpy(p5->words[p5->cw+30], cb);
   assert(Do_ext(p5));
   d1 = value(p5, "A");
   assert(double_cmp(d1, 50)==e);
   d1 = value(p5, "B");
   assert(double_cmp(d1, 8)==e);
   p5->cw++;

   p5->turt->x = 0;
   p5->turt->y = 0;
   p5->turt->theta = 0;
   p5->vars[0] = 0;
   strcpy(p5->words[p5->cw], do_lbl);
   strcpy(p5->words[p5->cw+1], "A");
   strcpy(p5->words[p5->cw+2], from);
   strcpy(p5->words[p5->cw+3], "1");
   strcpy(p5->words[p5->cw+4], to);
   strcpy(p5->words[p5->cw+5], "8");
   strcpy(p5->words[p5->cw+6], ob);
   strcpy(p5->words[p5->cw+7], fd1);
   strcpy(p5->words[p5->cw+8], "30");
   strcpy(p5->words[p5->cw+9], cb);
   strcpy(p5->words[p5->cw+10], cb);
   assert(Do_ext(p5));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 240.0));
   p5->cw++;

   /* Angles from y axis, i.e. LT 30 is 30 deg anti-clockwise from
   y axis */
   p5->turt->theta = 0.0;
   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   hypot = 30.0;
   new_pos(p5, hypot);
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 30.0));
   assert(!double_cmp(p5->turt->theta, 0.0));

   p5->turt->x = 0.0;
   p5->turt->y = 10.0;
   hypot = 30.0;
   assert(new_angle(p5, lt, 0.0));
   assert(new_pos(p5, hypot));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 40.0));
   assert(!double_cmp(p5->turt->theta, 0.0));

   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   hypot = 30.0;
   m = lt;
   assert(new_angle(p5, m, 90.0));
   assert(new_pos(p5, hypot));
   assert(!double_cmp(p5->turt->x, -30.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   assert(!double_cmp(p5->turt->theta, 90.0));

   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   hypot = 180.0;
   m = lt;
   assert(new_angle(p5, m, 180.0));
   assert(new_pos(p5, hypot));
   assert(!double_cmp(p5->turt->theta, 270.0));

   p5->turt->x = 15.0;
   p5->turt->y = 15.0;
   hypot = 30.0;
   m = lt;
   assert(new_angle(p5, m, 90.0));
   assert(new_pos(p5, hypot));
   assert(!double_cmp(p5->turt->theta, 360.0));

   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   hypot = 7.0;
   m = lt;
   assert(new_angle(p5, m, 360.0));
   assert(new_pos(p5, hypot));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 7.0));

   p5->turt->theta = 270.0;
   p5->turt->x = 10.0;
   p5->turt->y = 20.0;
   hypot = 30.0;
   m = lt;
   assert(new_angle(p5, m, 30.0));
   assert(new_pos(p5, hypot));
   assert(!double_cmp(p5->turt->x, 35.980762));
   assert(!double_cmp(p5->turt->y, 35.00));
   assert(!double_cmp(p5->turt->theta, 300.0));

   /* Move tests */
   p5->turt->theta = 30.0;
   p5->turt->x = 1.0;
   p5->turt->y = 1.0;
   p5->cw = 0;
   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "20");
   assert(Move_ext(p5));
   p5->cw++;

   p5->turt->theta = 0.0;
   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   p5->cw = 0;
   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "90");
   assert(Move_ext(p5));
   assert(!double_cmp(p5->turt->theta, 90.0));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "90");
   assert(Move_ext(p5));
   assert(!double_cmp(p5->turt->theta, 180.0));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "RT");
   strcpy(p5->words[p5->cw+1], "90");
   assert(Move_ext(p5));
   assert(!double_cmp(p5->turt->theta, 90.0));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "50");
   assert(Move_ext(p5));
   assert(!double_cmp(p5->turt->theta, 90.0));
   assert(!double_cmp(p5->turt->x, -50.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "RT");
   strcpy(p5->words[p5->cw+1], "180");
   assert(Move_ext(p5));
   assert(!double_cmp(p5->turt->theta, -90.0));
   assert(!double_cmp(p5->turt->x, -50.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "100");
   assert(Move_ext(p5));
   assert(!double_cmp(p5->turt->theta, -90.0));
   assert(!double_cmp(p5->turt->x, 50.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "RT");
   strcpy(p5->words[p5->cw+1], "180");
   assert(Move_ext(p5));
   assert(!double_cmp(p5->turt->theta, -270.0));
   assert(!double_cmp(p5->turt->x, 50.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "10");
   assert(Move_ext(p5));
   assert(!double_cmp(p5->turt->theta, -270.0));
   assert(!double_cmp(p5->turt->x, 40.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "10");
   assert(Instruction_ext(p5));
   assert(!double_cmp(p5->turt->theta, -270.0));
   assert(!double_cmp(p5->turt->x, 30.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "30");
   assert(Instruction_ext(p5));
   assert(!double_cmp(p5->turt->theta, -270.0));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "270");
   assert(Instruction_ext(p5));
   assert(!double_cmp(p5->turt->theta, 0.0));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 0.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "SET");
   strcpy(p5->words[p5->cw+1], "A");
   strcpy(p5->words[p5->cw+2], ":=");
   strcpy(p5->words[p5->cw+3], "30");
   strcpy(p5->words[p5->cw+4], ";");
   assert(Instruction_ext(p5));
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "A");
   assert(Instruction_ext(p5));
   assert(!double_cmp(p5->turt->theta, 0.0));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 30.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "A");
   assert(Instruction_ext(p5));
   assert(!double_cmp(p5->turt->theta, 30.0));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 30.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "A");
   assert(Instruction_ext(p5));
   assert(!double_cmp(p5->turt->theta, 60.0));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 30.0));
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "A");
   strcpy(p5->words[p5->cw+2], cb);
   assert(Instrctlst_ext(p5));
   assert(!double_cmp(p5->turt->theta, 90.0));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 30.0));
   p5->cw++;

   p5->turt->theta = 0.0;
   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   strcpy(p5->words[p5->cw], "DO");
   strcpy(p5->words[p5->cw+1], "A");
   strcpy(p5->words[p5->cw+2], from);
   strcpy(p5->words[p5->cw+3], "1");
   strcpy(p5->words[p5->cw+4], to);
   strcpy(p5->words[p5->cw+5], "360");
   strcpy(p5->words[p5->cw+6], ob);
   strcpy(p5->words[p5->cw+7], fd1);
   strcpy(p5->words[p5->cw+8], "10");
   strcpy(p5->words[p5->cw+9], lt1);
   strcpy(p5->words[p5->cw+10], "90");
   strcpy(p5->words[p5->cw+11], cb);
   strcpy(p5->words[p5->cw+12], cb);
   assert(Instrctlst_ext(p5));
   assert(!double_cmp(p5->turt->x, 0.0));
   assert(!double_cmp(p5->turt->y, 0.0));

   p5->turt->theta = 0.0;
   p5->turt->x = 10.001;
   p5->turt->y = 15.099;
   strcpy(p5->words[p5->cw], "DO");
   strcpy(p5->words[p5->cw+1], "A");
   strcpy(p5->words[p5->cw+2], from);
   strcpy(p5->words[p5->cw+3], "1");
   strcpy(p5->words[p5->cw+4], to);
   strcpy(p5->words[p5->cw+5], "40");
   strcpy(p5->words[p5->cw+6], ob);
   strcpy(p5->words[p5->cw+7], fd1);
   strcpy(p5->words[p5->cw+8], "10");
   strcpy(p5->words[p5->cw+9], lt1);
   strcpy(p5->words[p5->cw+10], "90");
   strcpy(p5->words[p5->cw+11], cb);
   strcpy(p5->words[p5->cw+12], cb);
   assert(Instrctlst_ext(p5));
   assert(!double_cmp(p5->turt->x, 10.001));
   assert(!double_cmp(p5->turt->y, 15.099));

   /* Used trigonometry here to check. Sin/cos */
   p5->turt->theta = 270.0;
   p5->turt->x = 2.0;
   p5->turt->y = 2.0;
   strcpy(p5->words[p5->cw], lt1);
   strcpy(p5->words[p5->cw+1], "30");
   strcpy(p5->words[p5->cw+2], fd1);
   strcpy(p5->words[p5->cw+3], "5");
   strcpy(p5->words[p5->cw+4], cb);
   assert(Instrctlst_ext(p5));
   assert(!double_cmp(p5->turt->y, 4.5));

   p5->turt->theta = 270.0;
   p5->turt->x = 2.0;
   p5->turt->y = 2.0;
   strcpy(p5->words[p5->cw], lt1);
   strcpy(p5->words[p5->cw+1], "60");
   strcpy(p5->words[p5->cw+2], fd1);
   strcpy(p5->words[p5->cw+3], "5");
   strcpy(p5->words[p5->cw+4], cb);
   assert(Instrctlst_ext(p5));
   assert(!double_cmp(p5->turt->x, 4.5));

   /* Examples from Question Description */
   /* Example 1 */
   strcpy(p6->words[p6->cw], ob);
   strcpy(p6->words[p6->cw+1], fd1);
   strcpy(p6->words[p6->cw+2], "30");
   strcpy(p6->words[p6->cw+3], lt1);
   strcpy(p6->words[p6->cw+4], "45");
   strcpy(p6->words[p6->cw+5], fd1);
   strcpy(p6->words[p6->cw+6], "30");
   strcpy(p6->words[p6->cw+7], lt1);
   strcpy(p6->words[p6->cw+8], "45");
   strcpy(p6->words[p6->cw+9], fd1);
   strcpy(p6->words[p6->cw+10], "30");
   strcpy(p6->words[p6->cw+11], lt1);
   strcpy(p6->words[p6->cw+12], "45");
   strcpy(p6->words[p6->cw+13], fd1);
   strcpy(p6->words[p6->cw+14], "30");
   strcpy(p6->words[p6->cw+15], lt1);
   strcpy(p6->words[p6->cw+16], "45");
   strcpy(p6->words[p6->cw+17], fd1);
   strcpy(p6->words[p6->cw+18], "30");
   strcpy(p6->words[p6->cw+19], lt1);
   strcpy(p6->words[p6->cw+20], "45");
   strcpy(p6->words[p6->cw+21], fd1);
   strcpy(p6->words[p6->cw+22], "30");
   strcpy(p6->words[p6->cw+23], lt1);
   strcpy(p6->words[p6->cw+24], "45");
   strcpy(p6->words[p6->cw+25], fd1);
   strcpy(p6->words[p6->cw+26], "30");
   strcpy(p6->words[p6->cw+27], lt1);
   strcpy(p6->words[p6->cw+28], "45");
   strcpy(p6->words[p6->cw+29], fd1);
   strcpy(p6->words[p6->cw+30], "30");
   strcpy(p6->words[p6->cw+31], lt1);
   strcpy(p6->words[p6->cw+32], "45");
   strcpy(p6->words[p6->cw+33], cb);
   assert(Main_ext(p6));
   p6->cw++;

   /* Adding Loops Example */
   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], do_lbl);
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], from);
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], to);
   strcpy(p4->words[p4->cw+6], "8");
   strcpy(p4->words[p4->cw+7], ob);
   strcpy(p4->words[p4->cw+8], fd1);
   strcpy(p4->words[p4->cw+9], "30");
   strcpy(p4->words[p4->cw+10], lt1);
   strcpy(p4->words[p4->cw+11], "45");
   strcpy(p4->words[p4->cw+12], cb);
   strcpy(p4->words[p4->cw+13], cb);
   assert(Main_ext(p4));
   p4->cw++;

   /* Using Variables Example */
   strcpy(p5->words[p5->cw], ob);
   strcpy(p5->words[p5->cw+1], do_lbl);
   strcpy(p5->words[p5->cw+2], "A");
   strcpy(p5->words[p5->cw+3], from);
   strcpy(p5->words[p5->cw+4], "1");
   strcpy(p5->words[p5->cw+5], to);
   strcpy(p5->words[p5->cw+6], "100");
   strcpy(p5->words[p5->cw+7], ob);
   strcpy(p5->words[p5->cw+8], set);
   strcpy(p5->words[p5->cw+9], "C");
   strcpy(p5->words[p5->cw+10], ":=");
   strcpy(p5->words[p5->cw+11], "A");
   strcpy(p5->words[p5->cw+12], "1.5");
   strcpy(p5->words[p5->cw+13], "*");
   strcpy(p5->words[p5->cw+14], sc);
   strcpy(p5->words[p5->cw+15], fd1);
   strcpy(p5->words[p5->cw+16], "C");
   strcpy(p5->words[p5->cw+17], rt1);
   strcpy(p5->words[p5->cw+18], "62");
   strcpy(p5->words[p5->cw+19], cb);
   strcpy(p5->words[p5->cw+20], cb);
   assert(Main_ext(p5));
   p5->cw++;

   /* Nested Loops Example */
   strcpy(p5->words[p5->cw], ob);
   strcpy(p5->words[p5->cw+1], do_lbl);
   strcpy(p5->words[p5->cw+2], "A");
   strcpy(p5->words[p5->cw+3], from);
   strcpy(p5->words[p5->cw+4], "1");
   strcpy(p5->words[p5->cw+5], to);
   strcpy(p5->words[p5->cw+6], "50");
   strcpy(p5->words[p5->cw+7], ob);
   strcpy(p5->words[p5->cw+8], fd1);
   strcpy(p5->words[p5->cw+9], "A");
   strcpy(p5->words[p5->cw+10], rt1);
   strcpy(p5->words[p5->cw+11], "30");
   strcpy(p5->words[p5->cw+12], do_lbl);
   strcpy(p5->words[p5->cw+13], "B");
   strcpy(p5->words[p5->cw+14], from);
   strcpy(p5->words[p5->cw+15], "1");
   strcpy(p5->words[p5->cw+16], to);
   strcpy(p5->words[p5->cw+17], "8");
   strcpy(p5->words[p5->cw+18], ob);
   strcpy(p5->words[p5->cw+19], set);
   strcpy(p5->words[p5->cw+20], "C");
   strcpy(p5->words[p5->cw+21], sign);
   strcpy(p5->words[p5->cw+22], "A");
   strcpy(p5->words[p5->cw+23], "5");
   strcpy(p5->words[p5->cw+24], "/");
   strcpy(p5->words[p5->cw+25], sc);
   strcpy(p5->words[p5->cw+26], fd1);
   strcpy(p5->words[p5->cw+27], "C");
   strcpy(p5->words[p5->cw+28], rt1);
   strcpy(p5->words[p5->cw+29], "45");
   strcpy(p5->words[p5->cw+30], cb);
   strcpy(p5->words[p5->cw+31], cb);
   strcpy(p5->words[p5->cw+32], cb);
   assert(Main_ext(p5));
   p5->cw++;

   strcpy(p5->words[p5->cw], ob);
   strcpy(p5->words[p5->cw+1], do_lbl);
   strcpy(p5->words[p5->cw+2], "A");
   strcpy(p5->words[p5->cw+3], from);
   strcpy(p5->words[p5->cw+4], "1");
   strcpy(p5->words[p5->cw+5], to);
   strcpy(p5->words[p5->cw+6], "50");
   strcpy(p5->words[p5->cw+7], ob);
   strcpy(p5->words[p5->cw+8], fd1);
   strcpy(p5->words[p5->cw+9], "A");
   strcpy(p5->words[p5->cw+10], rt1);
   strcpy(p5->words[p5->cw+11], "30");
   strcpy(p5->words[p5->cw+12], do_lbl);
   strcpy(p5->words[p5->cw+13], "B");
   strcpy(p5->words[p5->cw+14], from);
   strcpy(p5->words[p5->cw+15], "1");
   strcpy(p5->words[p5->cw+16], to);
   strcpy(p5->words[p5->cw+17], "8");
   strcpy(p5->words[p5->cw+18], ob);
   strcpy(p5->words[p5->cw+19], set);
   strcpy(p5->words[p5->cw+20], "C");
   strcpy(p5->words[p5->cw+21], sign);
   strcpy(p5->words[p5->cw+22], "A");
   strcpy(p5->words[p5->cw+23], "5");
   strcpy(p5->words[p5->cw+24], "/");
   strcpy(p5->words[p5->cw+25], sc);
   strcpy(p5->words[p5->cw+26], fd1);
   strcpy(p5->words[p5->cw+27], "C");
   strcpy(p5->words[p5->cw+28], rt1);
   strcpy(p5->words[p5->cw+29], "45");
   strcpy(p5->words[p5->cw+30], cb);
   strcpy(p5->words[p5->cw+31], cb);
   strcpy(p5->words[p5->cw+32], cb);
   assert(Main_ext(p5));
   p5->cw++;

   for(i = 0; i < MAXNUMTOKENS; i++){
      free(p2->words[i]);
      free(p3->words[i]);
      free(p4->words[i]);
      free(p5->words[i]);
      free(p6->words[i]);
      free(p7->words[i]);
   }

   free(p2->words);
   free(p3->words);
   free(p4->words);
   free(p5->words);
   free(p6->words);
   free(p7->words);

   free(p2->turt);
   free(p2->s);
   free(p2);

   free(p3->turt);
   free(p3->s);
   free(p3);

   free(p4->turt);
   free(p4->s);
   free(p4);

   free(p5->turt);
   free(p5->s);
   free(p5);

   free(p6->turt);
   free(p6->s);
   free(p6);

   free(p7->turt);
   free(p7->s);
   free(p7);

   stack_free(s);
}
