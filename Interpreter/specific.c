#include "specific.h"

int main(int argc, char* argv[])
{
   Program* p = ReadFile(argc, argv[1]);
   SDL_Setup(p);
   test();
   Main_interp(p);
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

bool Main_interp(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "{")){
         p->cw++;
         if(Instrctlst_interp(p)){
            return true;
         }
      }
   }
   conditional_err(p, "Expected a {");
   return false;
}

bool Instrctlst_interp(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "}")){
         return true;
      }
      if(Instruction_interp(p)){
         p->cw++;
         if(Instrctlst_interp(p)){
            return true;
         }
      }
   }
   conditional_err(p, "Expected a }, or Valid Instruction");
   return false;
}

bool Instruction_interp(Program *p)
{
   if(Move_interp(p) || Set_interp(p) || Do_interp(p)){
      return true;
   }
   conditional_err(p, "Expected a }, or Valid Instruction");
   return false;
}

bool Do_interp(Program *p)
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
                                 if(do_loop(p, max_loop_int, iter_var)){
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

bool do_loop(Program *p, double max, char iter_var[2])
{
   int current_word, final_word = 0;
   current_word = p->cw;
   while(value(p, iter_var) <= max){
      if(!Instrctlst_interp(p)){
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

bool Move_interp(Program *p)
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
         if(valid_movesize(p, m)){
            return true;
         }
      }
   }
   return false;
}

bool valid_movesize(Program *p, move m)
{
   p->cw++;
   if(access_vnum(p, p->words[p->cw])){
      if(assign_move(p, m)){
         return true;
      }
      return false;
   }
   conditional_err(p, "Expected Valid Variable or Number");
   return false;
}

bool assign_move(Program *p, move m)
{
   double size;
   size = value(p, p->words[p->cw]);
   if(m == fd){
      if(new_pos(p, size)){
         if(!p->tflag){
            draw_line(p);
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

void draw_line(Program *p)
{
   SDL_Delay(MILLISECONDDELAY);
   SDL_RenderDrawLine(p->sw.renderer, p->turt->prev_x, p->turt->prev_y, \
   p->turt->x, p->turt->y);
   Neill_SDL_UpdateScreen(&p->sw);
   Neill_SDL_Events(&p->sw);
}

void test(void)
{
   int i;
   Program* p2 = prog_init(MAXNUMTOKENS);
   Program* p3 = prog_init(MAXNUMTOKENS);
   Program* p4 = prog_init(MAXNUMTOKENS);
   Program* p5 = prog_init(MAXNUMTOKENS);
   Program* p6 = prog_init(MAXNUMTOKENS);
   Program* p7 = prog_init(MAXNUMTOKENS);
   stack *s = stack_init();
   int x = 0, newx = 0;

   double d1;
   double d2;
   double epsilon = 0.000001;
   double n_epsilon = -0.000001;
   double diff;
   double hypot;

   char* fd1 = "FD";
   char* fd2 = "fd";
   char* fd3 = "Fd";
   char* lt1 = "LT";
   char* lt2 = "lt";
   char* rt1 = "RT";
   char* rt2 = "rt";
   char* rt3 = "rT";
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
   char* c3 = "Z";
   char* c4 = "X";
   char* c5 = "AA";
   char* c6 = "Bb";

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
   char *num14 = ".";
   /*Should we allow this?*/
   char *num15 = ".456";
   char *num16 = "1..456";
   char *num18 = "-2";
   char *num22 = ".-123";
   char *num25 = "1 23";
   char *num26 = "-102";
   move m = none;

   /* Prog_init tests */
   assert(p2);
   assert(p2->s);
   assert(p2->words);
   assert(p2->turt);
   assert(p3);
   assert(p4);
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

   assert(!check_endpoints(3002, 1));
/* SECTION 1 */
/*  ///////////////////////////////
    /// SAME TESTS AS IN PARSER ///
    ///   USING INTERP VERSION  ///
    ///////////////////////////////
*/

   /* Testing Varnum function works correctly, also Move_interp function will return
   false here, since not preceding a varnum, increasing counter each time */
   strcpy(p2->words[p2->cw], num1);
   assert(Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   assert(!Instruction_interp(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw], num14);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   assert(!Instruction_interp(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw], num22);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   assert(!Instruction_interp(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw], num10);
   assert(Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   assert(!Instruction_interp(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw], num11);
   assert(Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   assert(!Instruction_interp(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw], num12);
   assert(Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   assert(!Instruction_interp(p2));
   p2->cw++;

   /*Checking the Move_interp function with FD, will return true when the string "FD"
   is followed by a Varnum*/
   /*Also testing Instruction_interp function, which returns true when Move_interp returns
   true*/
   strcpy(p2->words[p2->cw], fd1);
   strcpy(p2->words[p2->cw+1], num1);
   assert(!Varnum(p2->words[p2->cw]));
   x = p2->cw;
   assert(Move_interp(p2));
   p2->cw--;
   assert(Instruction_interp(p2));
   newx = p2->cw;
   /*Testing counter increases*/
   assert(newx - x == 1);
   assert(!strcmp(p2->words[p2->cw],num1));

   set_value(p2, "A", 0);
   strcpy(p2->words[p2->cw], fd1);
   strcpy(p2->words[p2->cw+1], num25);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   p2->cw--;
   assert(!Instruction_interp(p2));

   strcpy(p2->words[p2->cw], fd2);
   strcpy(p2->words[p2->cw+1], num1);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   p2->cw--;
   assert(!Instruction_interp(p2));

   strcpy(p2->words[p2->cw], fd3);
   strcpy(p2->words[p2->cw+1], num1);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   p2->cw--;
   assert(!Instruction_interp(p2));

   /*Checking the Move_interp function with LT, will return true when the string "LT"
   is followed by a Varnum*/
   strcpy(p2->words[p2->cw], lt1);
   strcpy(p2->words[p2->cw+1], num1);
   assert(!Varnum(p2->words[p2->cw]));
   x = p2->cw;
   assert(Move_interp(p2));
   p2->cw--;
   assert(Instruction_interp(p2));
   newx = p2->cw;
   /*Testing counter increases*/
   newx = x + 1;
   assert(!strcmp(p2->words[p2->cw],num1));

   strcpy(p2->words[p2->cw], lt1);
   strcpy(p2->words[p2->cw+1], num25);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   p2->cw--;
   assert(!Instruction_interp(p2));

   strcpy(p2->words[p2->cw], lt2);
   strcpy(p2->words[p2->cw+1], num1);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   p2->cw--;
   assert(!Instruction_interp(p2));

   /*Checking the Move_interp function with RT, will return true when the string "RT"
   is followed by a Varnum*/
   strcpy(p2->words[p2->cw], rt1);
   strcpy(p2->words[p2->cw+1], num1);
   assert(!Varnum(p2->words[p2->cw]));
   x = p2->cw;
   assert(Move_interp(p2));
   p2->cw--;
   assert(Instruction_interp(p2));
   newx = p2->cw;
   /*Testing counter increases*/
   newx = x + 1;
   assert(!strcmp(p2->words[p2->cw],num1));

   strcpy(p2->words[p2->cw], rt1);
   strcpy(p2->words[p2->cw+1], num25);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   p2->cw--;
   assert(!Instruction_interp(p2));

   strcpy(p2->words[p2->cw], rt2);
   strcpy(p2->words[p2->cw+1], num1);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   p2->cw--;
   assert(!Instruction_interp(p2));

   strcpy(p2->words[p2->cw], rt3);
   strcpy(p2->words[p2->cw+1], num1);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move_interp(p2));
   p2->cw--;
   assert(!Instruction_interp(p2));

   /*Testing the Instrctlst_interp function*/
   strcpy(p2->words[p2->cw], cb);
   assert(Instrctlst_interp(p2));

   strcpy(p2->words[p2->cw], "}");
   assert(Instrctlst_interp(p2));

   strcpy(p2->words[p2->cw], ")");
   assert(!Instrctlst_interp(p2));

   strcpy(p2->words[p2->cw], fd1);
   strcpy(p2->words[p2->cw+1], num18);
   strcpy(p2->words[p2->cw+2], cb);
   assert(Instrctlst_interp(p2));

   strcpy(p2->words[p2->cw], fd1);
   strcpy(p2->words[p2->cw+1], num18);
   assert(!Instrctlst_interp(p2));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num18);
   assert(!Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], cb);
   assert(!Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   assert(!Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num3);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num26);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num8);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num10);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num9);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num12);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num13);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num2);
   strcpy(p3->words[p3->cw+2], lt1);
   strcpy(p3->words[p3->cw+3], num9);
   strcpy(p3->words[p3->cw+4], rt1);
   strcpy(p3->words[p3->cw+5], num10);
   strcpy(p3->words[p3->cw+6], cb);
   assert(Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num2);
   strcpy(p3->words[p3->cw+2], lt1);
   strcpy(p3->words[p3->cw+3], num9);
   strcpy(p3->words[p3->cw+4], lt1);
   strcpy(p3->words[p3->cw+5], num10);
   strcpy(p3->words[p3->cw+6], lt1);
   strcpy(p3->words[p3->cw+7], num10);
   strcpy(p3->words[p3->cw+8], cb);
   assert(Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], fd1);
   strcpy(p3->words[p3->cw+1], num2);
   strcpy(p3->words[p3->cw+2], num15);
   strcpy(p3->words[p3->cw+3], cb);
   assert(!Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], rt1);
   strcpy(p3->words[p3->cw+1], num16);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Instrctlst_interp(p3));

   strcpy(p3->words[p3->cw], rt1);
   strcpy(p3->words[p3->cw+1], num16);
   assert(!Instrctlst_interp(p3));

   /*Testing the Main_interp function*/
   strcpy(p3->words[p3->cw], ob);
   assert(!Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num16);
   strcpy(p3->words[p3->cw+3], cb);
   assert(!Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num3);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num4);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num11);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_interp(p3));

   strcpy(p3->words[p3->cw], rt1);
   strcpy(p3->words[p3->cw+1], num13);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], rt1);
   strcpy(p3->words[p3->cw+3], num13);
   strcpy(p3->words[p3->cw+4], cb);
   assert(!Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt1);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], fd1);
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt1);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], "fd!");
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(!Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt1);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], "fd");
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(!Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt1);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], "Fd");
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(!Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt1);
   strcpy(p3->words[p3->cw+2], num13);
   assert(!Main_interp(p3));

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], cb);
   assert(Main_interp(p3));

   /*//////*/

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
   assert(!Instruction_interp(p4));
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
   assert(!Instrctlst_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c4);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], num1);
   strcpy(p4->words[p4->cw+5], "+");
   strcpy(p4->words[p4->cw+6], sc);
   strcpy(p4->words[p4->cw+7], cb);
   assert(Instrctlst_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c4);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], num1);
   strcpy(p4->words[p4->cw+5], "+");
   strcpy(p4->words[p4->cw+6], sc);
   assert(!Instrctlst_interp(p4));
   p4->cw++;



   /* SECTION 2:
   ///////////////////////////////////////
   ///////// INTERPRETER TESTS ///////////
   ///////////////////////////////////////
   */

   /* Testing is_set_var */
   p2->var_set[0] = false;
   p2->var_set[1] = false;
   p2->var_set[2] = false;
   p2->var_set[3] = false;
   assert(!is_set_var(p2, "A"));
   assert(!is_set_var(p2, "B"));
   assert(!is_set_var(p2, "C"));
   assert(!is_set_var(p2, "D"));
   assert(!is_set_var(p2, "E"));
   assert(!is_set_var(p2, "e"));
   assert(!is_set_var(p2, "-19239"));
   p2->var_set[0] = true;
   p2->var_set[1] = true;
   p2->var_set[2] = true;
   p2->var_set[3] = true;
   assert(is_set_var(p2, "A"));
   assert(is_set_var(p2, "B"));
   assert(is_set_var(p2, "C"));
   assert(is_set_var(p2, "D"));

   /* Testing access_vnum */
   assert(!access_vnum(p7, "A"));
   assert(!access_vnum(p7, "B"));
   assert(!access_vnum(p7, "C"));
   assert(!access_vnum(p7, "d"));
   assert(!access_vnum(p7, "d"));
   assert(access_vnum(p7, "12"));
   assert(!access_vnum(p7, "A111"));
   assert(!access_vnum(p7, "..."));
   p7->var_set[0] = true;
   p7->var_set[1] = true;
   p7->var_set[2] = true;
   p7->var_set[3] = true;
   assert(access_vnum(p7, "A"));
   assert(access_vnum(p7, "B"));
   assert(access_vnum(p7, "C"));
   assert(access_vnum(p7, "D"));
   assert(!access_vnum(p7, "a"));
   assert(!access_vnum(p7, "b"));
   assert(!access_vnum(p7, "c"));
   assert(!access_vnum(p7, "d"));
   assert(!access_vnum(p7, "Dd"));
   assert(!access_vnum(p7, "Ad"));
   assert(!access_vnum(p7, "?!"));
   p7->var_set[0] = false;
   p7->var_set[1] = false;
   assert(!access_vnum(p7, "A"));
   assert(!access_vnum(p7, "B"));

   /* Testing result_op */

   d1 = 1.0;
   d2 = 2.0;
   d1 = result_op(d1, d2, "+");
   diff = d1 - 3.0;
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = 100.0;
   d2 = 25.0;
   d1 = result_op(d1, d2, "/");
   diff = d1 - 0.25;
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = 5.0;
   d2 = 0.5;
   d1 = result_op(d1, d2, "/");
   diff = d1 - 0.1;
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = 1.0;
   d2 = 10.0;
   d1 = result_op(d1, d2, "*");
   diff = d1 - 10.0;
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = 1.5;
   d2 = 3.0;
   d1 = result_op(d1, d2, "*");
   diff = d1 - 4.5;
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = 0.0;
   d2 = 3.0;
   d1 = result_op(d1, d2, "*");
   diff = d1 - 0.0;
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = 99.9999;
   d2 = 99.0;
   d1 = result_op(d1, d2, "-");
   diff = d1 - 0.9999;
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = -15.0;
   d2 = 10.0;
   d1 = result_op(d1, d2, "+");
   diff = d1 - (-5.0);
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = -3.5;
   d2 = -0.5;
   d1 = result_op(d1, d2, "/");
   diff = d1 - 0.142857;
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = -0.005;
   d2 = 900000;
   d1 = result_op(d1, d2, "*");
   diff = d1 - (-4500);
   assert(diff <= epsilon && diff >= n_epsilon);

   d1 = -0.5;
   d2 = -0.5;
   d1 = result_op(d1, d2, "/");
   diff = d1 - (1.0);
   assert(diff <= epsilon && diff >= n_epsilon);

   /* double compare tests */
   d1 = 0.0;
   d2 = 0.0;
   assert(double_cmp(d1, d2)==e);
   d1 = 1.0;
   d2 = 1.0;
   assert(double_cmp(d1, d2)==e);
   d1 = 99.999;
   d2 = 99.999;
   assert(double_cmp(d1, d2)==e);
   d1 = 1.0;
   d2 = 0.0;
   assert(double_cmp(d1, d2)==g);
   d1 = 1.999;
   d2 = -3.999;
   assert(double_cmp(d1, d2)==g);
   d1 = -1.999;
   d2 = -3.999;
   assert(double_cmp(d1, d2)==g);
   d1 = 10.999;
   d2 = -11.999;
   assert(double_cmp(d1, d2)==g);
   d1 = 0.000;
   d2 = -0.001;
   assert(double_cmp(d1, d2)==g);
   d1 = 0.1;
   d2 = 3.0;
   assert(double_cmp(d1, d2)==l);
   d1 = -13.999;
   d2 = -3.999;
   assert(double_cmp(d1, d2)==l);
   d1 = -1029.999;
   d2 = -1028.999;
   assert(double_cmp(d1, d2)==l);
   d1 = -10.999;
   d2 = 11.999;
   assert(double_cmp(d1, d2)==l);
   d1 = 0.000;
   d2 = 0.001;
   assert(double_cmp(d1, d2)==l);

   /* Checking count stack */
   d1 = 20.5;
   d2 = 29.9;
   assert(count_stack(s)==0);
   stack_push(s, d1);
   assert(count_stack(s)==1);
   stack_push(s, d2);
   stack_push(s, d2);
   stack_push(s, d2);
   assert(count_stack(s)==4);
   stack_pop(s, &d1);
   assert(count_stack(s)==3);
   stack_pop(s, &d2);
   assert(count_stack(s)==2);
   stack_pop(s, &d2);
   assert(count_stack(s)==1);
   stack_pop(s, &d2);
   assert(!count_stack(s));

   /* Checking count stack does not change stack */
   d1 = 1;
   d2 = 2;
   stack_push(s, d1);
   stack_push(s, d2);
   d1 = 3;
   d2 = 4;
   stack_push(s, d1);
   stack_push(s, d2);
   assert(count_stack(s)==4);

   stack_pop(s, &d1);
   assert(count_stack(s)==3);
   diff = d1 - (4.0);
   assert(diff <= epsilon && diff >= n_epsilon);

   stack_pop(s, &d1);
   assert(count_stack(s)==2);
   diff = d1 - (3.0);
   assert(diff <= epsilon && diff >= n_epsilon);

   stack_pop(s, &d1);
   assert(count_stack(s)==1);
   diff = d1 - (2.0);
   assert(diff <= epsilon && diff >= n_epsilon);

   stack_pop(s, &d1);
   assert(count_stack(s)==0);
   diff = d1 - (1.0);
   assert(diff <= epsilon && diff >= n_epsilon);

   /* Checking PUSH/POP as well as result_op */
   /* Also checking count_stack */
   /* Testing polish_oper */
   stack_push(p7->s, 5.5);
   stack_push(p7->s, 0.1);
   strcpy(p7->words[p7->cw], "+");
   strcpy(p7->words[p7->cw+1], ";");
   assert(Polish_oper(p7));
   stack_pop(p7->s, &d1);
   p7->cw++;

   stack_push(p7->s, 5.5);
   stack_push(p7->s, 0.001);
   strcpy(p7->words[p7->cw], "*");
   strcpy(p7->words[p7->cw+1], ";");
   assert(Polish_oper(p7));
   stack_pop(p7->s, &d1);
   p7->cw++;

   stack_push(p7->s, 5.5);
   stack_push(p7->s, 0.0);
   strcpy(p7->words[p7->cw], "/");
   strcpy(p7->words[p7->cw+1], ";");
   assert(!Polish_oper(p7));
   stack_pop(p7->s, &d1);
   p7->cw++;

   stack_push(p7->s, 0.1);
   stack_push(p7->s, 1.0);
   strcpy(p7->words[p7->cw], "/");
   strcpy(p7->words[p7->cw+1], ";");
   assert(Polish_oper(p7));
   stack_pop(p7->s, &d1);
   p7->cw++;

   /* Polish _vnum tests */
   p7->var_set[0] = false;
   strcpy(p7->words[p7->cw], "A");
   assert(!Polish_vnum(p7));
   stack_pop(p7->s, &d1);
   p7->cw++;

   p6->var_set[0] = true;
   p6->vars[0] = 10.0;
   strcpy(p6->words[p6->cw], "A");
   strcpy(p6->words[p6->cw+1], ";");
   assert(Polish_vnum(p6));
   p6->cw++;

   p6->var_set[0] = true;
   p6->vars[0] = 10.0;
   strcpy(p6->words[p6->cw], "A");
   assert(!Polish_vnum(p6));
   p6->cw++;

   assert(!count_stack(p5->s));
   strcpy(p5->words[p5->cw], num1);
   strcpy(p5->words[p5->cw+1], num1);
   strcpy(p5->words[p5->cw+2], "+");
   strcpy(p5->words[p5->cw+3], ";");
   assert(Polish_interp(p5));
   assert(double_cmp(p5->new_var_value, 2.0)==e);
   stack_pop(p5->s, &d1);
   assert(!count_stack(p5->s));
   p5->cw++;

   strcpy(p5->words[p5->cw], "100");
   strcpy(p5->words[p5->cw+1], "10");
   strcpy(p5->words[p5->cw+2], "/");
   strcpy(p5->words[p5->cw+3], ";");
   assert(Polish_interp(p5));
   assert(double_cmp(p5->new_var_value, 10.0)==e);
   stack_pop(p5->s, &d1);
   assert(!count_stack(p5->s));
   p5->cw++;

   strcpy(p5->words[p5->cw], "2.5");
   strcpy(p5->words[p5->cw+1], "2.0");
   strcpy(p5->words[p5->cw+2], "/");
   strcpy(p5->words[p5->cw+3], ";");
   assert(Polish_interp(p5));
   stack_pop(p7->s, &d1);
   assert(double_cmp(p5->new_var_value, 1.25)==e);
   stack_pop(p5->s, &d1);
   assert(!count_stack(p5->s));
   p5->cw++;

   strcpy(p5->words[p5->cw], num1);
   strcpy(p5->words[p5->cw+1], num2);
   strcpy(p5->words[p5->cw+2], "/");
   strcpy(p5->words[p5->cw+3], ";");
   assert(Polish_interp(p5));
   stack_pop(p7->s, &d1);
   assert(double_cmp(p5->new_var_value, 0.1)==e);
   stack_pop(p5->s, &d1);
   assert(!count_stack(p5->s));
   p5->cw++;

   strcpy(p5->words[p5->cw], "-0.5");
   strcpy(p5->words[p5->cw+1], "-0.5");
   strcpy(p5->words[p5->cw+2], "/");
   strcpy(p5->words[p5->cw+3], ";");
   assert(Polish_interp(p5));
   stack_pop(p7->s, &d1);
   assert(double_cmp(p5->new_var_value, 1.0)==e);
   stack_pop(p5->s, &d1);
   assert(!count_stack(p5->s));
   p5->cw++;

   strcpy(p5->words[p5->cw], "0.0001");
   strcpy(p5->words[p5->cw+1], "100000");
   strcpy(p5->words[p5->cw+2], "+");
   strcpy(p5->words[p5->cw+3], ";");
   assert(Polish_interp(p5));
   stack_pop(p7->s, &d1);
   assert(double_cmp(p5->new_var_value, 100000.0001)==e);
   stack_pop(p5->s, &d1);
   assert(!count_stack(p5->s));
   p5->cw++;

   strcpy(p5->words[p5->cw], "0.09");
   strcpy(p5->words[p5->cw+1], "-0.091");
   strcpy(p5->words[p5->cw+2], "+");
   strcpy(p5->words[p5->cw+3], ";");
   assert(Polish_interp(p5));
   stack_pop(p7->s, &d1);
   assert(double_cmp(p5->new_var_value, -0.001)==e);
   stack_pop(p5->s, &d1);
   assert(!count_stack(p5->s));
   p5->cw++;

   strcpy(p5->words[p5->cw], "5");
   strcpy(p5->words[p5->cw+1], "10");
   strcpy(p5->words[p5->cw+2], "15");
   strcpy(p5->words[p5->cw+3], "20");
   strcpy(p5->words[p5->cw+4], "+");
   strcpy(p5->words[p5->cw+5], "-");
   strcpy(p5->words[p5->cw+6], "-");
   strcpy(p5->words[p5->cw+7], ";");
   assert(Polish_interp(p5));
   stack_pop(p7->s, &d1);
   assert(double_cmp(p5->new_var_value, 20.0)==e);
   stack_pop(p5->s, &d1);
   assert(!count_stack(p5->s));
   p5->cw++;

   strcpy(p5->words[p5->cw], "-100");
   strcpy(p5->words[p5->cw+1], "10");
   strcpy(p5->words[p5->cw+2], "10");
   strcpy(p5->words[p5->cw+3], "10");
   strcpy(p5->words[p5->cw+4], "/");
   strcpy(p5->words[p5->cw+5], "/");
   strcpy(p5->words[p5->cw+6], "/");
   strcpy(p5->words[p5->cw+7], ";");
   assert(Polish_interp(p5));
   assert(double_cmp(p5->new_var_value, -10.0)==e);
   stack_pop(p5->s, &d1);
   assert(!count_stack(p5->s));
   p5->cw++;

   stack_pop(p4->s, &d1);
   stack_pop(p4->s, &d1);
   strcpy(p4->words[p4->cw], sc);
   assert(!Polish_interp(p4));
   assert(!count_stack(p4->s));


   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], sc);
   assert(!Polish_interp(p4));
   assert(!count_stack(p4->s));

   strcpy(p4->words[p4->cw], "+");
   assert(!Polish_interp(p4));
   assert(!count_stack(p4->s));

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], "-");
   strcpy(p4->words[p4->cw+2], "+");
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Polish_interp(p4));
   assert(!count_stack(p4->s));

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], "*");
   strcpy(p4->words[p4->cw+2], "?");
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Polish_interp(p4));
   assert(!count_stack(p4->s));

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], "*");
   strcpy(p4->words[p4->cw+2], sc);
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Polish_interp(p4));
   assert(!count_stack(p4->s));

   strcpy(p4->words[p4->cw], "*");
   strcpy(p4->words[p4->cw+1], "/");
   assert(!Polish_interp(p4));
   assert(!count_stack(p4->s));

   strcpy(p4->words[p4->cw], "*");
   strcpy(p4->words[p4->cw+1], "/");
   assert(!Polish_interp(p4));

   strcpy(p4->words[p4->cw], c2);
   strcpy(p4->words[p4->cw+1], sc);
   assert(!Polish_interp(p4));

   strcpy(p4->words[p4->cw], c1);
   strcpy(p4->words[p4->cw+1], sc);
   assert(!Polish_interp(p4));
   p4->cw++;

   p4->cw = 0;
   strcpy(p4->words[p4->cw], c2);
   assert(!Polish_interp(p4));

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], "+");
   strcpy(p4->words[p4->cw+2], "+");
   strcpy(p4->words[p4->cw+3], "+");
   strcpy(p4->words[p4->cw+4], ";");
   assert(!Polish_interp(p4));

   strcpy(p4->words[p4->cw], sc);
   strcpy(p4->words[p4->cw+1], sc);
   assert(!Polish_interp(p4));

   strcpy(p4->words[p4->cw], "/");
   strcpy(p4->words[p4->cw], sc);
   assert(!Polish_interp(p4));

   strcpy(p4->words[p4->cw], num1);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], c4);
   strcpy(p4->words[p4->cw+3], num13);
   strcpy(p4->words[p4->cw+4], num18);
   strcpy(p4->words[p4->cw+5], sc);
   assert(!Polish_interp(p4));
   stack_pop(p4->s, &d1);
   stack_pop(p4->s, &d1);

   strcpy(p4->words[p4->cw], num1);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], c4);
   strcpy(p4->words[p4->cw+3], num14);
   strcpy(p4->words[p4->cw+4], num18);
   strcpy(p4->words[p4->cw+5], sc);
   assert(!Polish_interp(p4));

   strcpy(p4->words[p4->cw], num9);
   strcpy(p4->words[p4->cw+1], c5);
   strcpy(p4->words[p4->cw+2], sc);
   assert(!Polish_interp(p4));
   p4->cw = 0;
   stack_pop(p4->s, &d1);
   stack_pop(p4->s, &d1);

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], num3);
   strcpy(p4->words[p4->cw+2], sc);
   assert(!Polish_interp(p4));
   p4->cw++;
   stack_pop(p4->s, &d1);
   stack_pop(p4->s, &d1);

   strcpy(p4->words[p4->cw], num3);
   strcpy(p4->words[p4->cw+1], num3);
   strcpy(p4->words[p4->cw+2], "+");
   strcpy(p4->words[p4->cw+3], "+");
   strcpy(p4->words[p4->cw+4], sc);
   assert(!Polish_interp(p4));
   p4->cw++;
   stack_pop(p4->s, &d1);
   stack_pop(p4->s, &d1);

   /* Zero_check Tests */
   assert(zero_err(p7, "/", 0.0));
   assert(!zero_err(p7, "/", 1.0));
   assert(!zero_err(p7, "/", 0.0002));
   assert(!zero_err(p7, "/", -0.0002));
   assert(!zero_err(p7, "*", 0.0));


/* ////////////////////////////////// */
/* Changes to SET function */
/* First, testing set_value in histogram */
/* Checking set_value works correctly by
   using the access_vnum function, and value */

   p5->var_to_be_set = "A";
   d1 = p5->vars[0];
   diff = d1 - 0.0;
   assert(diff <= epsilon && diff >= n_epsilon);
   p5->new_var_value = 1.0;
   set_value(p5, p5->var_to_be_set, p5->new_var_value);
   assert(access_vnum(p5, "A"));
   d1 = value(p5, "A");
   assert(double_cmp(d1, 1.0)==e);

   p5->var_to_be_set = "C";
   d1 = p5->vars[2];
   diff = d1 - 0.0;
   assert(diff <= epsilon && diff >= n_epsilon);
   p5->new_var_value = 99.012;
   set_value(p5, p5->var_to_be_set, p5->new_var_value);
   assert(access_vnum(p5, "C"));
   d1 = value(p5, "C");
   assert(double_cmp(d1, 99.012)==e);

   p5->var_to_be_set = "Z";
   d1 = p5->vars[25];
   diff = d1 - 0.0;
   assert(diff <= epsilon && diff >= n_epsilon);
   p5->new_var_value = 3.1;
   set_value(p5, p5->var_to_be_set, p5->new_var_value);
   assert(access_vnum(p5, "Z"));
   d1 = value(p5, "Z");
   assert(double_cmp(d1, 3.1)==e);

   p5->var_to_be_set = "A";
   d1 = p5->vars[0];
   diff = d1 - 1.0;
   assert(diff <= epsilon && diff >= n_epsilon);
   p5->new_var_value = d1 + 2;
   set_value(p5, p5->var_to_be_set, p5->new_var_value);
   assert(access_vnum(p5, "A"));
   d1 = value(p5, "A");
   assert(double_cmp(d1, 3.0)==e);

   p5->var_to_be_set = "A";
   d1 = p5->vars[0];
   diff = d1 - 3.0;
   assert(diff <= epsilon && diff >= n_epsilon);
   p5->new_var_value = result_op(d1, 2.0, "*");
   set_value(p5, p5->var_to_be_set, p5->new_var_value);
   assert(access_vnum(p5, "A"));
   d1 = value(p5, "A");
   assert(double_cmp(d1, 6.0)==e);

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c3);
   strcpy(p4->words[p4->cw+2], sign);
   assert(!Set_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], c3);
   strcpy(p4->words[p4->cw+1], sign);
   strcpy(p4->words[p4->cw+2], "/");
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Set_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], c2);
   strcpy(p4->words[p4->cw+1], sign);
   strcpy(p4->words[p4->cw+2], c2);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], c4);
   strcpy(p4->words[p4->cw+5], num13);
   strcpy(p4->words[p4->cw+6], num18);
   strcpy(p4->words[p4->cw+7], set);
   strcpy(p4->words[p4->cw+8], sc);
   strcpy(p4->words[p4->cw+9], num13);
   assert(!Set_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], c2);
   strcpy(p4->words[p4->cw+1], sc);
   assert(!Set_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c6);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Set_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "Set");
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Set_interp(p4));
   p4->cw++;

   p7->var_set[0] = true;
   p7->cw = 0;
   strcpy(p7->words[p7->cw], "SET");
   strcpy(p7->words[p7->cw+1], "A");
   strcpy(p7->words[p7->cw+2], sign);
   strcpy(p7->words[p7->cw+3], "100");
   strcpy(p7->words[p7->cw+4], "10");
   strcpy(p7->words[p7->cw+5], "/");
   strcpy(p7->words[p7->cw+6], ";");
   assert(Set_interp(p7));
   p7->cw++;

   strcpy(p7->words[p7->cw], "SET");
   strcpy(p7->words[p7->cw+1], "A");
   strcpy(p7->words[p7->cw+2], sign);
   strcpy(p7->words[p7->cw+3], "B");
   strcpy(p7->words[p7->cw+4], "100");
   strcpy(p7->words[p7->cw+5], "10");
   strcpy(p7->words[p7->cw+6], "/");
   strcpy(p7->words[p7->cw+7], "+");
   strcpy(p7->words[p7->cw+8], ";");
   assert(!Set_interp(p7));
   p7->cw++;

   stack_pop(p6->s, &d1);
   stack_pop(p6->s, &d1);
   stack_pop(p6->s, &d1);
   stack_pop(p6->s, &d1);

   p6->cw = 30;
   set_value(p6, "B", 0.0);
   set_value(p6, "A", 0.0);
   strcpy(p6->words[p6->cw], "SET");
   strcpy(p6->words[p6->cw+1], "A");
   strcpy(p6->words[p6->cw+2], sign);
   strcpy(p6->words[p6->cw+3], "B");
   strcpy(p6->words[p6->cw+4], "100");
   strcpy(p6->words[p6->cw+5], "10");
   strcpy(p6->words[p6->cw+6], "/");
   strcpy(p6->words[p6->cw+7], "+");
   strcpy(p6->words[p6->cw+8], ";");
   assert(Set_interp(p6));
   d1 = value(p6, "A");
   assert(double_cmp(d1, 10.0)==e);
   assert(access_vnum(p6, "A"));
   p6->cw++;

   stack_pop(p6->s, &d1);
   strcpy(p6->words[p6->cw], "SET");
   strcpy(p6->words[p6->cw+1], "A");
   strcpy(p6->words[p6->cw+2], sign);
   strcpy(p6->words[p6->cw+3], "A");
   strcpy(p6->words[p6->cw+4], "2");
   strcpy(p6->words[p6->cw+5], "10");
   strcpy(p6->words[p6->cw+6], "*");
   strcpy(p6->words[p6->cw+7], "+");
   strcpy(p6->words[p6->cw+8], ";");
   assert(Set_interp(p6));
   d1 = value(p6, "A");
   assert(double_cmp(d1, 30.0)==e);
   assert(access_vnum(p6, "A"));
   p6->cw++;

   stack_pop(p6->s, &d1);
   set_value(p6, "Z", 0.0);
   set_value(p6, "A", 0.0);
   set_value(p6, "B", 0.0);
   strcpy(p6->words[p6->cw], "SET");
   strcpy(p6->words[p6->cw+1], "A");
   strcpy(p6->words[p6->cw+2], sign);
   strcpy(p6->words[p6->cw+3], "B");
   strcpy(p6->words[p6->cw+4], "2");
   strcpy(p6->words[p6->cw+5], "10");
   strcpy(p6->words[p6->cw+6], "*");
   strcpy(p6->words[p6->cw+7], "+");
   strcpy(p6->words[p6->cw+8], "Z");
   strcpy(p6->words[p6->cw+9], "9");
   strcpy(p6->words[p6->cw+10], "+");
   strcpy(p6->words[p6->cw+11], "3");
   strcpy(p6->words[p6->cw+12], "-");
   strcpy(p6->words[p6->cw+13], "-");
   strcpy(p6->words[p6->cw+14], ";");
   assert(Set_interp(p6));
   assert(access_vnum(p6, "A"));
   p6->cw++;

   stack_pop(p6->s, &d1);
   strcpy(p6->words[p6->cw], "SET");
   strcpy(p6->words[p6->cw+1], "A");
   strcpy(p6->words[p6->cw+2], sign);
   strcpy(p6->words[p6->cw+3], "B");
   strcpy(p6->words[p6->cw+4], "2");
   strcpy(p6->words[p6->cw+5], "0");
   strcpy(p6->words[p6->cw+6], "*");
   strcpy(p6->words[p6->cw+7], "+");
   strcpy(p6->words[p6->cw+8], "Z");
   strcpy(p6->words[p6->cw+9], "Z");
   strcpy(p6->words[p6->cw+10], "+");
   strcpy(p6->words[p6->cw+11], "3");
   strcpy(p6->words[p6->cw+12], "-");
   strcpy(p6->words[p6->cw+13], "-");
   strcpy(p6->words[p6->cw+14], ";");
   assert(Set_interp(p6));
   assert(access_vnum(p6, "A"));
   p6->cw++;

/* Can't divide by zero */
   p6->var_set[0] = true;
   p6->var_set[1] = true;

   stack_pop(p6->s, &d1);
   p6->cw = 75;
   strcpy(p6->words[p6->cw], "SET");
   strcpy(p6->words[p6->cw+1], "A");
   strcpy(p6->words[p6->cw+2], sign);
   strcpy(p6->words[p6->cw+3], "0");
   strcpy(p6->words[p6->cw+4], "2");
   strcpy(p6->words[p6->cw+5], "/");
   strcpy(p6->words[p6->cw+6], ";");
   assert(Set_interp(p6));
   p6->cw++;

   stack_pop(p6->s, &d1);
   strcpy(p6->words[p6->cw], "SET");
   strcpy(p6->words[p6->cw+1], "A");
   strcpy(p6->words[p6->cw+2], sign);
   strcpy(p6->words[p6->cw+3], "B");
   strcpy(p6->words[p6->cw+4], "0");
   strcpy(p6->words[p6->cw+5], "/");
   strcpy(p6->words[p6->cw+6], ";");
   assert(!Set_interp(p6));
   p6->cw++;
   stack_pop(p6->s, &d1);
   stack_pop(p6->s, &d1);

   strcpy(p6->words[p6->cw], "SET");
   strcpy(p6->words[p6->cw+1], "A");
   strcpy(p6->words[p6->cw+2], sign);
   strcpy(p6->words[p6->cw+3], "2");
   strcpy(p6->words[p6->cw+4], "0");
   strcpy(p6->words[p6->cw+5], ";");
   assert(!Set_interp(p6));
   p6->cw++;
   stack_pop(p6->s, &d1);
   stack_pop(p6->s, &d1);

   strcpy(p6->words[p6->cw], "SET");
   strcpy(p6->words[p6->cw+1], "A");
   strcpy(p6->words[p6->cw+2], sign);
   strcpy(p6->words[p6->cw+3], "B");
   strcpy(p6->words[p6->cw+4], "2");
   strcpy(p6->words[p6->cw+5], "10");
   strcpy(p6->words[p6->cw+6], "*");
   strcpy(p6->words[p6->cw+7], "+");
   strcpy(p6->words[p6->cw+8], "Z");
   strcpy(p6->words[p6->cw+9], "Z");
   strcpy(p6->words[p6->cw+10], "+");
   strcpy(p6->words[p6->cw+11], "3");
   strcpy(p6->words[p6->cw+12], "-");
   strcpy(p6->words[p6->cw+13], ";");
   assert(!Set_interp(p6));
   p6->cw++;
    stack_pop(p6->s, &d1);
    stack_pop(p6->s, &d1);


   strcpy(p6->words[p6->cw], "SET");
   strcpy(p6->words[p6->cw+1], "A");
   strcpy(p6->words[p6->cw+2], sign);
   strcpy(p6->words[p6->cw+3], "B");
   strcpy(p6->words[p6->cw+4], "2");
   strcpy(p6->words[p6->cw+5], "2");
   strcpy(p6->words[p6->cw+6], "10");
   strcpy(p6->words[p6->cw+7], "*");
   strcpy(p6->words[p6->cw+8], "+");
   strcpy(p6->words[p6->cw+9], "Z");
   strcpy(p6->words[p6->cw+10], "Z");
   strcpy(p6->words[p6->cw+11], "+");
   strcpy(p6->words[p6->cw+12], "3");
   strcpy(p6->words[p6->cw+13], "-");
   strcpy(p6->words[p6->cw+14], "-");
   strcpy(p6->words[p6->cw+15], ";");
   assert(!Set_interp(p6));
   p6->cw++;
   stack_pop(p6->s, &d1);
   stack_pop(p6->s, &d1);


/* /////////////////////////////////// */

/* DO TESTS */

/* Checking endpoints - only do the DO loop
   if FROM < TO */
   assert(check_endpoints(9.0, 10.0));
   assert(check_endpoints(-9.0, 10.0));
   assert(check_endpoints(9.999, 10.0));
   assert(!check_endpoints(10.0, 9.9999));
   assert(check_endpoints(9.0, 9.0));
   assert(check_endpoints(-3.333, -3.33));
   assert(!check_endpoints(100.0, 10.0));
   assert(!check_endpoints(3002, 1));

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
   assert(!Do_interp(p4));
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
   assert(Do_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], cb);
   assert(!Do_interp(p4));
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
   assert(!Do_interp(p4));
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
   assert(Do_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], from);
   strcpy(p4->words[p4->cw+2], num3);
   strcpy(p4->words[p4->cw+3], to);
   strcpy(p4->words[p4->cw+4], num1);
   strcpy(p4->words[p4->cw+5], ob);
   strcpy(p4->words[p4->cw+6], cb);
   assert(!Do_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], to);
   strcpy(p4->words[p4->cw+4], num3);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], cb);
   assert(!Do_interp(p4));
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
   assert(!Do_interp(p4));
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
   assert(!Do_interp(p4));
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
   assert(!Do_interp(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num9);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num10);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], cb);
   assert(Do_interp(p4));
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
   assert(Do_interp(p4));
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
   assert(!Do_interp(p4));
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
   assert(!Do_interp(p4));
   p4->cw++;

   /* Testing incrementing correctly */
   /* B will be one more, do A from 1 to 8, 8 included...  */
   p5->vars[1] = 0;
   strcpy(p5->words[p5->cw], do_lbl);
   strcpy(p5->words[p5->cw+1], "B");
   strcpy(p5->words[p5->cw+2], from);
   strcpy(p5->words[p5->cw+3], "1");
   strcpy(p5->words[p5->cw+4], to);
   strcpy(p5->words[p5->cw+5], "10");
   strcpy(p5->words[p5->cw+6], ob);
   strcpy(p5->words[p5->cw+7], cb);
   assert(Do_interp(p5));
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
   assert(Do_interp(p5));
   d1 = value(p5, "B");
   assert(double_cmp(d1, 10.5)==e);
   p5->cw++;

   set_value(p5, "D", 0.0);
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
   assert(Do_interp(p5));
   d1 = value(p5, "B");
   assert(double_cmp(d1, 99)==e);
   p5->cw++;

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
   assert(Do_interp(p5));
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
   assert(Do_interp(p5));
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
   assert(Do_interp(p5));
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 240.0)==e);
   p5->cw++;

   /* Cant do DO loops where TO is unset variable */
   assert(!access_vnum(p7, "E"));
   strcpy(p7->words[p7->cw], do_lbl);
   strcpy(p7->words[p7->cw+1], "A");
   strcpy(p7->words[p7->cw+2], from);
   strcpy(p7->words[p7->cw+3], "1");
   strcpy(p7->words[p7->cw+4], to);
   strcpy(p7->words[p7->cw+5], "E");
   strcpy(p7->words[p7->cw+6], ob);
   strcpy(p7->words[p7->cw+7], fd1);
   strcpy(p7->words[p7->cw+8], "30");
   strcpy(p7->words[p7->cw+9], cb);
   strcpy(p7->words[p7->cw+10], cb);
   assert(!Do_interp(p7));
   p7->cw++;

   set_value(p7, "E", 10.01921);
   strcpy(p7->words[p7->cw], do_lbl);
   strcpy(p7->words[p7->cw+1], "A");
   strcpy(p7->words[p7->cw+2], from);
   strcpy(p7->words[p7->cw+3], "1");
   strcpy(p7->words[p7->cw+4], to);
   strcpy(p7->words[p7->cw+5], "E");
   strcpy(p7->words[p7->cw+6], ob);
   strcpy(p7->words[p7->cw+7], fd1);
   strcpy(p7->words[p7->cw+8], "30");
   strcpy(p7->words[p7->cw+9], cb);
   strcpy(p7->words[p7->cw+10], cb);
   assert(Do_interp(p7));
   p7->cw++;


   /* do_loop tests - passes if valid instruction set */
   set_value(p7, "A", 5.0);
   strcpy(p7->words[p7->cw], cb);
   assert(do_loop(p7, 10.0, "A"));
   assert(double_cmp(value(p7, "A"), 10.0)==e);
   p7->cw++;

   set_value(p7, "A", 10.0);
   strcpy(p7->words[p7->cw], "FD");
   strcpy(p7->words[p7->cw+1], "-2");
   strcpy(p7->words[p7->cw+2], "LT");
   strcpy(p7->words[p7->cw+3], "RT");
   strcpy(p7->words[p7->cw+4], cb);
   strcpy(p7->words[p7->cw+5], cb);
   assert(!do_loop(p7, 11.0, "A"));
   p7->cw++;

   set_value(p7, "A", 10.0);
   strcpy(p7->words[p7->cw], "FD");
   strcpy(p7->words[p7->cw+1], "-2");
   strcpy(p7->words[p7->cw+2], "LT");
   strcpy(p7->words[p7->cw+3], "20");
   strcpy(p7->words[p7->cw+4], cb);
   assert(do_loop(p7, 11.0, "A"));
   assert(double_cmp(value(p7, "A"), 11.0)==e);
   p7->cw++;

   /* Valid do loop, where start = max, runs 1 iteration */
   set_value(p7, "A", 10.0);
   strcpy(p7->words[p7->cw], "FD");
   strcpy(p7->words[p7->cw+1], "-2");
   strcpy(p7->words[p7->cw+2], "LT");
   strcpy(p7->words[p7->cw+3], "20");
   strcpy(p7->words[p7->cw+4], cb);
   assert(do_loop(p7, 10.0, "A"));
   assert(double_cmp(value(p7, "A"), 10.0)==e);
   p7->cw++;



   /* Position testing */
   /* New_angle tests */
   p5->turt->theta = 0.0;
   m = lt;
   assert(new_angle(p5, m, 30.0));
   assert(double_cmp(p5->turt->theta, 30.0)==e);

   assert(new_angle(p5, m, 30.0));
   assert(double_cmp(p5->turt->theta, 60.0)==e);

   assert(new_angle(p5, m, 95.1020));
   assert(double_cmp(p5->turt->theta, 155.1020)==e);
   m = rt;
   assert(new_angle(p5, m, 55.1020));
   assert(double_cmp(p5->turt->theta, 100.0)==e);

   assert(new_angle(p5, m, 99.9));
   assert(double_cmp(p5->turt->theta, 0.1)==e);
   m = lt;
   assert(new_angle(p5, m, 359.9));
   assert(double_cmp(p5->turt->theta, 360.0)==e);

   assert(new_angle(p5, m, 180.0));
   assert(double_cmp(p5->turt->theta, 540.0)==e);
   m = rt;
   assert(new_angle(p5, m, -0.0001));
   assert(double_cmp(p5->turt->theta, 540.0001)==e);
   m = lt;
   assert(new_angle(p5, m, -0.0001));
   assert(double_cmp(p5->turt->theta, 540.000)==e);

   /* Angles from y axis, i.e. LT 30 is 30 deg anti-clockwise from
   y axis */
   /* new_pos tests */
   p5->turt->theta = 0.0;
   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   hypot = 30.0;
   new_pos(p5, hypot);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 30.0)==e);
   assert(double_cmp(p5->turt->theta, 0.0)==e);

   p5->turt->x = 0.0;
   p5->turt->y = 10.0;
   hypot = 30.0;
   m = lt;
   assert(new_angle(p5, m, 0.0));
   assert(new_pos(p5, hypot));
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 40.0)==e);
   assert(double_cmp(p5->turt->theta, 0.0)==e);

   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   hypot = 30.0;
   assert(new_angle(p5, lt, 90.0));
   assert(new_pos(p5, hypot));
   assert(double_cmp(p5->turt->x, -30.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   assert(double_cmp(p5->turt->theta, 90.0)==e);


   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   hypot = 180.0;
   assert(new_angle(p5, lt, 180.0));
   assert(new_pos(p5, hypot));
   assert(double_cmp(p5->turt->theta, 270.0)==e);

   p5->turt->x = 15.0;
   p5->turt->y = 15.0;
   hypot = 30.0;
   assert(new_angle(p5, lt, 90.0));
   assert(new_pos(p5, hypot));
   assert(double_cmp(p5->turt->theta, 360.0)==e);

   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   hypot = 7.0;
   assert(new_angle(p5, lt, 360.0));
   assert(new_pos(p5, hypot));
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 7.0)==e);

   p5->turt->theta = 270.0;
   p5->turt->x = 10.0;
   p5->turt->y = 20.0;
   hypot = 30.0;
   assert(new_angle(p5, lt, 30.0));
   assert(new_pos(p5, hypot));
   assert(double_cmp(p5->turt->x, 35.980762)==e);
   assert(double_cmp(p5->turt->y, 35.00)==e);
   assert(double_cmp(p5->turt->theta, 300.0)==e);

   /* Assign_move tests */
   assert(assign_move(p7, lt));
   assert(assign_move(p7, rt));
   assert(assign_move(p7, fd));
   assert(assign_move(p7, fd));
   assert(assign_move(p7, fd));
   assert(!assign_move(p7, none));


   /* Move_interp tests */
   p5->turt->theta = 30.0;
   p5->turt->x = 1.0;
   p5->turt->y = 1.0;
   p5->cw = 0;
   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "20");
   assert(Move_interp(p5));
   p5->cw++;

   p5->turt->theta = 0.0;
   p5->turt->x = 0.0;
   p5->turt->y = 0.0;
   p5->cw = 0;
   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "90");
   assert(Move_interp(p5));
   assert(double_cmp(p5->turt->theta, 90.0)==e);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "90");
   assert(Move_interp(p5));
   assert(double_cmp(p5->turt->theta, 180.0)==e);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "RT");
   strcpy(p5->words[p5->cw+1], "90");
   assert(Move_interp(p5));
   assert(double_cmp(p5->turt->theta, 90.0)==e);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "50");
   assert(Move_interp(p5));
   assert(double_cmp(p5->turt->theta, 90.0)==e);
   assert(double_cmp(p5->turt->x, -50.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "RT");
   strcpy(p5->words[p5->cw+1], "180");
   assert(Move_interp(p5));
   assert(double_cmp(p5->turt->theta, -90.0)==e);
   assert(double_cmp(p5->turt->x, -50.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "100");
   assert(Move_interp(p5));
   assert(double_cmp(p5->turt->theta, -90.0)==e);
   assert(double_cmp(p5->turt->x, 50.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "RT");
   strcpy(p5->words[p5->cw+1], "180");
   assert(Move_interp(p5));
   assert(double_cmp(p5->turt->theta, -270.0)==e);
   assert(double_cmp(p5->turt->x, 50.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "10");
   assert(Move_interp(p5));
   assert(double_cmp(p5->turt->theta, -270.0)==e);
   assert(double_cmp(p5->turt->x, 40.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   /* Cant move by an unset variable */
   p5->var_set[0] = false;
   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "A");
   assert(!Move_interp(p5));
   p5->cw++;

   p5->var_set[0] = true;
   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "A");
   assert(Move_interp(p5));
   p5->cw++;
   p5->cw++;

   p5->turt->x = 40.0;
   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "10");
   assert(Instruction_interp(p5));
   assert(double_cmp(p5->turt->theta, -270.0)==e);
   assert(double_cmp(p5->turt->x, 30.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "30");
   assert(Instruction_interp(p5));
   assert(double_cmp(p5->turt->theta, -270.0)==e);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "270");
   assert(Instruction_interp(p5));
   assert(double_cmp(p5->turt->theta, 0.0)==e);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "SET");
   strcpy(p5->words[p5->cw+1], "A");
   strcpy(p5->words[p5->cw+2], ":=");
   strcpy(p5->words[p5->cw+3], "30");
   strcpy(p5->words[p5->cw+4], ";");
   assert(Instruction_interp(p5));
   p5->cw++;

   strcpy(p5->words[p5->cw], "FD");
   strcpy(p5->words[p5->cw+1], "A");
   assert(Instruction_interp(p5));
   assert(double_cmp(p5->turt->theta, 0.0)==e);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 30.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "A");
   assert(Instruction_interp(p5));
   assert(double_cmp(p5->turt->theta, 30.0)==e);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 30.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "A");
   assert(Instruction_interp(p5));
   assert(double_cmp(p5->turt->theta, 60.0)==e);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 30.0)==e);
   p5->cw++;

   strcpy(p5->words[p5->cw], "LT");
   strcpy(p5->words[p5->cw+1], "A");
   strcpy(p5->words[p5->cw+2], cb);
   assert(Instrctlst_interp(p5));
   assert(double_cmp(p5->turt->theta, 90.0)==e);
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 30.0)==e);
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
   assert(Instrctlst_interp(p5));
   assert(double_cmp(p5->turt->x, 0.0)==e);
   assert(double_cmp(p5->turt->y, 0.0)==e);

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
   assert(Instrctlst_interp(p5));
   assert(double_cmp(p5->turt->x, 10.001)==e);
   assert(double_cmp(p5->turt->y, 15.099)==e);

   /* Used trigonometry here to check. Sin/cos */
   p5->turt->theta = 270.0;
   p5->turt->x = 2.0;
   p5->turt->y = 2.0;
   strcpy(p5->words[p5->cw], lt1);
   strcpy(p5->words[p5->cw+1], "30");
   strcpy(p5->words[p5->cw+2], fd1);
   strcpy(p5->words[p5->cw+3], "5");
   strcpy(p5->words[p5->cw+4], cb);
   assert(Instrctlst_interp(p5));
   assert(double_cmp(p5->turt->y, 4.5)==e);

   p5->turt->theta = 270.0;
   p5->turt->x = 2.0;
   p5->turt->y = 2.0;
   strcpy(p5->words[p5->cw], lt1);
   strcpy(p5->words[p5->cw+1], "60");
   strcpy(p5->words[p5->cw+2], fd1);
   strcpy(p5->words[p5->cw+3], "5");
   strcpy(p5->words[p5->cw+4], cb);
   assert(Instrctlst_interp(p5));
   assert(double_cmp(p5->turt->x, 4.5)==e);

   /* Examples from Question Description */
   /* Example 1 */
   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], fd1);
   strcpy(p4->words[p4->cw+2], "30");
   strcpy(p4->words[p4->cw+3], lt1);
   strcpy(p4->words[p4->cw+4], "45");
   strcpy(p4->words[p4->cw+5], fd1);
   strcpy(p4->words[p4->cw+6], "30");
   strcpy(p4->words[p4->cw+7], lt1);
   strcpy(p4->words[p4->cw+8], "45");
   strcpy(p4->words[p4->cw+9], fd1);
   strcpy(p4->words[p4->cw+10], "30");
   strcpy(p4->words[p4->cw+11], lt1);
   strcpy(p4->words[p4->cw+12], "45");
   strcpy(p4->words[p4->cw+13], fd1);
   strcpy(p4->words[p4->cw+14], "30");
   strcpy(p4->words[p4->cw+15], lt1);
   strcpy(p4->words[p4->cw+16], "45");
   strcpy(p4->words[p4->cw+17], fd1);
   strcpy(p4->words[p4->cw+18], "30");
   strcpy(p4->words[p4->cw+19], lt1);
   strcpy(p4->words[p4->cw+20], "45");
   strcpy(p4->words[p4->cw+21], fd1);
   strcpy(p4->words[p4->cw+22], "30");
   strcpy(p4->words[p4->cw+23], lt1);
   strcpy(p4->words[p4->cw+24], "45");
   strcpy(p4->words[p4->cw+25], fd1);
   strcpy(p4->words[p4->cw+26], "30");
   strcpy(p4->words[p4->cw+27], lt1);
   strcpy(p4->words[p4->cw+28], "45");
   strcpy(p4->words[p4->cw+29], fd1);
   strcpy(p4->words[p4->cw+30], "30");
   strcpy(p4->words[p4->cw+31], lt1);
   strcpy(p4->words[p4->cw+32], "45");
   strcpy(p4->words[p4->cw+33], cb);
   assert(Main_interp(p4));
   p4->cw++;

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
   assert(Main_interp(p4));
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
   assert(Main_interp(p5));
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
   assert(Main_interp(p5));
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
   assert(Main_interp(p5));
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
