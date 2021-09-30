#include "parser.h"

int main(int argc, char* argv[])
{
   Program* p = ReadFile(argc, argv[1]);
   test();
   Main_gram(p);
   p->cw++;
   if(p->cw != p->numwords){
      on_error("Error: Text after Main.");
   }
   free_all(p);
   return 0;
}

void free_all(Program *p)
{
   unsigned int i = 0;
   for(i = 0; i < p->numwords; i++){
      free(p->words[i]);
   }
   free(p->words);
   free(p);
}

Program* prog_init(unsigned int word_cnt)
{
   Program* p = (Program *)ncalloc(1, sizeof(Program));
   p->words = (char**)ncalloc(word_cnt, sizeof(char *));
   p->numwords = word_cnt;
   return p;
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

bool Main_gram(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "{")){
         p->cw++;
         if(Instrctlst(p)){
            return true;
         }
      }
   }
   conditional_err(p, "Expected a {");
   return false;
}

bool Instrctlst(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "}")){
         return true;
      }
      if(Instruction(p)){
         p->cw++;
         if(Instrctlst(p)){
            return true;
         }
      }
   }
   conditional_err(p, "Expected a }, or Valid Instruction");
   return false;
}

bool Instruction(Program *p)
{
   if(Move(p) || Set(p) || Do(p)){
      return true;
   }
   conditional_err(p, "Expected a }, or Valid Instruction");
   return false;
}

void conditional_err(Program *p, char *err)
{
   if(!p->tflag){
      on_error(err);
   }
}

bool Do(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "DO")){
         p->cw++;
         if(is_var(p->words[p->cw++])){
            if(!strcmp(p->words[p->cw++], "FROM")){
               if(Varnum(p->words[p->cw++])){
                  if(!strcmp(p->words[p->cw++], "TO")){
                     if(Varnum(p->words[p->cw++])){
                        if(!strcmp(p->words[p->cw++], "{")){
                           if(Instrctlst(p)){
                              return true;
                           }
                        }
                        conditional_err(p, "Expected {");
                     }
                     conditional_err(p, "Expected Variable or Number");
                  }
                  conditional_err(p, "Expected TO");
               }
               conditional_err(p, "Expected Variable or Number");
            }
            conditional_err(p, "Expected FROM");
         }
         conditional_err(p, "Expected Variable");
      }
   }
   return false;
}

bool Move(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(strcmp(p->words[p->cw], "FD") && strcmp(p->words[p->cw], "LT") && \
      strcmp(p->words[p->cw], "RT")){
         return false;
      }
      p->cw++;
      if(!Varnum(p->words[p->cw])){
         conditional_err(p, "Expected Valid Variable or Number");
         return false;
      }
      return true;
   }
   return false;
}

bool Set(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], "SET")){
         p->cw++;
         if(is_var(p->words[p->cw++])){
            if(!strcmp(p->words[p->cw++], ":=")){
               if(Polish(p)){
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

bool Polish(Program *p)
{
   if(strlen(p->words[p->cw])){
      if(!strcmp(p->words[p->cw], ";")){
         return true;
      }
      if(is_op(p->words[p->cw])){
         p->cw++;
         if(Polish(p)){
            return true;
         }
      }
      if(Varnum(p->words[p->cw])){
         p->cw++;
         if(Polish(p)){
            return true;
         }
      }
   }
   conditional_err(p, "Expected Valid Variable, Number, or ;");
   return false;
}

void test(void)
{
   Program* p2 = prog_init(MAXNUMTOKENS);
   Program* p3 = prog_init(MAXNUMTOKENS);
   Program* p4 = prog_init(MAXNUMTOKENS);
   Program* p5 = prog_init(MAXNUMTOKENS);
   int x = 0, newx = 0, i;

   char* fd = "FD";
   char* fd2 = "fd";
   char* fd3 = "Fd";
   char* lt = "LT";
   char* lt2 = "lt";
   char* rt = "RT";
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
   char* c7 = "1";
   char* c8 = "?";
   char* c9 = "+";
   char* c10 = "-";
   char* c11 = "*";
   char* c12 = "/";
   char* c13 = "_";
   char* c14 = "++";
   char* c15 = "+-";

   char *num1 = "1";
   char *num2 = "10";
   char *num3 = "3002";
   char *num4 = "010";
   char *num5 = "str";
   char *num6 = "sh0uldnt";
   char *num7 = "1w0rk";
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
   char *num17 = "1456.";
   char *num18 = "-2";
   char *num19 = "-";
   char *num20 = "--";
   char *num21 = "-1.23";
   char *num22 = ".-123";
   char *num23 = "12-3";
   char *num24 = "123-";
   char *num25 = "1 23";
   char *num26 = "one";
   char *num27 = "a";
   char *num29 = "-.";
   char *num30 = "0";

   for(i = 0; i < MAXNUMTOKENS; i++){
      p2->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
      p3->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
      p4->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
      p5->words[i] = (char *)ncalloc(MAXTOKENLEN, sizeof(char));
   }

   p2->tflag = true;
   p3->tflag = true;
   p4->tflag = true;
   p5->tflag = true;

   assert(is_num(num1));
   assert(is_num(num2));
   assert(is_num(num3));
   assert(is_num(num4));
   assert(!is_num(num5));
   assert(!is_num(num6));
   assert(!is_num(num7));
   assert(!is_num(num8));
   assert(is_num(num9));
   assert(is_num(num10));
   assert(is_num(num11));
   assert(is_num(num12));
   assert(is_num(num13));
   assert(!is_num(num14));
   assert(!is_num(num15));
   assert(!is_num(num16));
   assert(is_num(num17));
   assert(is_num(num18));
   assert(!is_num(num19));
   assert(!is_num(num20));
   assert(is_num(num21));
   assert(!is_num(num22));
   assert(!is_num(num23));
   assert(!is_num(num24));
   assert(!is_num(num25));
   assert(!is_num(num26));
   assert(!is_num(num27));
   assert(!is_num(".-"));
   assert(!is_num(num29));
   assert(is_num(num30));

   /* Testing Varnum function works correctly */
   assert(Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   assert(!Instruction(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw],num14);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   assert(!Instruction(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw],num22);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   assert(!Instruction(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw],num10);
   assert(Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   assert(!Instruction(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw],num11);
   assert(Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   assert(!Instruction(p2));
   p2->cw++;
   strcpy(p2->words[p2->cw],num12);
   assert(Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   assert(!Instruction(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],c11);
   assert(!Varnum(p2->words[p2->cw]));
   p2->cw++;
   strcpy(p2->words[p2->cw],c1);
   assert(!Varnum(p2->words[p2->cw]));
   p2->cw++;
   strcpy(p2->words[p2->cw],c6);
   assert(!Varnum(p2->words[p2->cw]));
   p2->cw++;
   strcpy(p2->words[p2->cw],c4);
   assert(Varnum(p2->words[p2->cw]));
   p2->cw++;
   strcpy(p2->words[p2->cw],c2);
   assert(Varnum(p2->words[p2->cw]));
   p2->cw++;
   strcpy(p2->words[p2->cw],num8);
   assert(!Varnum(p2->words[p2->cw]));
   p2->cw++;

   /* Testing the Move function */
   /* Also testing Instruction function, which returns true when Move returns
   true */
   strcpy(p2->words[p2->cw],fd);
   strcpy(p2->words[p2->cw+1],num1);
   assert(!Varnum(p2->words[p2->cw]));
   x = p2->cw;
   assert(Move(p2));
   p2->cw--;
   assert(Instruction(p2));
   newx = p2->cw;
   /*Testing counter increases*/
   assert(newx - x == 1);
   assert(!strcmp(p2->words[p2->cw],num1));

   strcpy(p2->words[p2->cw],fd);
   strcpy(p2->words[p2->cw+1],num25);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   p2->cw--;
   assert(!Instruction(p2));

   strcpy(p2->words[p2->cw],fd2);
   strcpy(p2->words[p2->cw+1],num1);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   p2->cw--;
   assert(!Instruction(p2));

   strcpy(p2->words[p2->cw],fd3);
   strcpy(p2->words[p2->cw+1],num1);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   p2->cw--;
   assert(!Instruction(p2));

   strcpy(p2->words[p2->cw],fd);
   strcpy(p2->words[p2->cw+1],"A");
   x = p2->cw;
   assert(Move(p2));
   p2->cw--;
   assert(Instruction(p2));
   newx = p2->cw;
   /*Testing counter increases*/
   assert(newx - x == 1);
   assert(!strcmp(p2->words[p2->cw],"A"));

   strcpy(p2->words[p2->cw],fd);
   strcpy(p2->words[p2->cw+1],"a");
   assert(!Move(p2));
   p2->cw--;
   assert(!Instruction(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],fd);
   strcpy(p2->words[p2->cw+1],"Y");
   assert(Move(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],lt);
   strcpy(p2->words[p2->cw+1],num1);
   assert(!Varnum(p2->words[p2->cw]));
   x = p2->cw;
   assert(Move(p2));
   p2->cw--;
   assert(Instruction(p2));
   newx = p2->cw;
   /*Testing counter increases*/
   newx = x + 1;
   assert(!strcmp(p2->words[p2->cw],num1));

   strcpy(p2->words[p2->cw],lt);
   strcpy(p2->words[p2->cw+1],num25);
   assert(!Move(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],lt);
   strcpy(p2->words[p2->cw+1],num13);
   assert(Move(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],lt);
   strcpy(p2->words[p2->cw+1],num21);
   assert(Move(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],lt);
   strcpy(p2->words[p2->cw+1],"Z");
   assert(Move(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],lt2);
   strcpy(p2->words[p2->cw+1],"Z");
   assert(!Move(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],rt);
   strcpy(p2->words[p2->cw+1],num1);
   assert(!Varnum(p2->words[p2->cw]));
   x = p2->cw;
   assert(Move(p2));
   p2->cw--;
   assert(Instruction(p2));
   newx = p2->cw;
   /*Testing counter increases*/
   newx = x + 1;
   assert(!strcmp(p2->words[p2->cw],num1));
   p2->cw++;

   strcpy(p2->words[p2->cw],rt);
   strcpy(p2->words[p2->cw+1],num25);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   p2->cw--;
   assert(!Instruction(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],rt2);
   strcpy(p2->words[p2->cw+1],num1);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   p2->cw--;
   assert(!Instruction(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],rt3);
   strcpy(p2->words[p2->cw+1],num1);
   assert(!Varnum(p2->words[p2->cw]));
   assert(!Move(p2));
   p2->cw--;
   assert(!Instruction(p2));
   p2->cw++;

   strcpy(p2->words[p2->cw],rt);
   strcpy(p2->words[p2->cw+1],"Z");
   assert(Move(p2));
   p2->cw++;

   /*Testing the Instrctlst function*/
   strcpy(p2->words[p2->cw], cb);
   assert(Instrctlst(p2));

   strcpy(p2->words[p2->cw], "}");
   assert(Instrctlst(p2));

   strcpy(p2->words[p2->cw], ")");
   assert(!Instrctlst(p2));

   strcpy(p2->words[p2->cw], fd);
   strcpy(p2->words[p2->cw+1], num18);
   strcpy(p2->words[p2->cw+2], cb);
   assert(Instrctlst(p2));

   strcpy(p2->words[p2->cw], fd);
   strcpy(p2->words[p2->cw+1], "x");
   assert(!Instrctlst(p2));

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], num22);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Instrctlst(p3));

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], num18);
   assert(!Instrctlst(p3));

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], cb);
   assert(!Instrctlst(p3));

   strcpy(p3->words[p3->cw], fd);
   assert(!Instrctlst(p3));

   strcpy(p3->words[p3->cw], lt);
   strcpy(p3->words[p3->cw+1], "A");
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst(p3));

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], num8);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Instrctlst(p3));

   strcpy(p3->words[p3->cw], rt);
   strcpy(p3->words[p3->cw+1], "D");
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst(p3));

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], num30);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst(p3));

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], num12);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst(p3));

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], num13);
   strcpy(p3->words[p3->cw+2], cb);
   assert(Instrctlst(p3));

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], num2);
   strcpy(p3->words[p3->cw+2], lt);
   strcpy(p3->words[p3->cw+3], num9);
   strcpy(p3->words[p3->cw+4], rt);
   strcpy(p3->words[p3->cw+5], num10);
   strcpy(p3->words[p3->cw+6], cb);
   assert(Instrctlst(p3));

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], num2);
   strcpy(p3->words[p3->cw+2], lt);
   strcpy(p3->words[p3->cw+3], num9);
   strcpy(p3->words[p3->cw+4], lt);
   strcpy(p3->words[p3->cw+5], num10);
   strcpy(p3->words[p3->cw+6], lt);
   strcpy(p3->words[p3->cw+7], num10);
   strcpy(p3->words[p3->cw+8], cb);
   assert(Instrctlst(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], rt);
   strcpy(p3->words[p3->cw+1], "A");
   strcpy(p3->words[p3->cw+2], lt);
   strcpy(p3->words[p3->cw+3], "A");
   strcpy(p3->words[p3->cw+4], lt);
   strcpy(p3->words[p3->cw+5], "0.000012");
   strcpy(p3->words[p3->cw+6], lt);
   strcpy(p3->words[p3->cw+7], num10);
   strcpy(p3->words[p3->cw+8], cb);
   assert(Instrctlst(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], rt);
   strcpy(p3->words[p3->cw+1], "A");
   strcpy(p3->words[p3->cw+2], lt);
   strcpy(p3->words[p3->cw+3], "A");
   strcpy(p3->words[p3->cw+4], lt);
   strcpy(p3->words[p3->cw+5], "0.000012");
   strcpy(p3->words[p3->cw+6], lt);
   strcpy(p3->words[p3->cw+7], num10);
   strcpy(p3->words[p3->cw+8], cb);
   assert(Instrctlst(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], fd);
   strcpy(p3->words[p3->cw+1], num2);
   strcpy(p3->words[p3->cw+2], num15);
   strcpy(p3->words[p3->cw+3], cb);
   assert(!Instrctlst(p3));

   strcpy(p3->words[p3->cw], rt);
   strcpy(p3->words[p3->cw+1], num16);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Instrctlst(p3));

   strcpy(p3->words[p3->cw], rt);
   strcpy(p3->words[p3->cw+1], num16);
   assert(!Instrctlst(p3));

   /*Testing the Main_gram function*/
   strcpy(p3->words[p3->cw], ob);
   assert(!Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num16);
   strcpy(p3->words[p3->cw+3], cb);
   assert(!Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num3);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num4);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num11);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], cb);
   assert(Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], rt);
   strcpy(p3->words[p3->cw+1], num13);
   strcpy(p3->words[p3->cw+2], cb);
   assert(!Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], rt);
   strcpy(p3->words[p3->cw+3], num13);
   strcpy(p3->words[p3->cw+4], cb);
   assert(!Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], fd);
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], "fd!");
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(!Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], "fd");
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(!Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num13);
   strcpy(p3->words[p3->cw+3], lt);
   strcpy(p3->words[p3->cw+4], num12);
   strcpy(p3->words[p3->cw+5], "Fd");
   strcpy(p3->words[p3->cw+6], num13);
   strcpy(p3->words[p3->cw+7], cb);
   assert(!Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], rt);
   strcpy(p3->words[p3->cw+2], num13);
   assert(!Main_gram(p3));
   p3->cw++;

   strcpy(p3->words[p3->cw], ob);
   strcpy(p3->words[p3->cw+1], cb);
   assert(Main_gram(p3));
   p3->cw++;

   /*///////////////////////////////////*/

   assert(!is_var(c1));
   assert(is_var(c2));
   assert(is_var(c3));
   assert(is_var(c4));
   assert(!is_var(c5));
   assert(!is_var(c6));
   assert(!is_var(c7));
   assert(!is_var(c8));
   assert(!is_var(c9));
   assert(!is_var(c10));
   assert(!is_var(num1));
   assert(!is_var(num2));
   assert(!is_var(num10));
   assert(!is_var(c11));
   assert(!is_var(c12));
   assert(!is_var(c13));
   assert(!is_var(c14));
   assert(!is_var(c15));

   assert(!is_op(c2));
   assert(!is_op(c3));
   assert(!is_op(c7));
   assert(!is_op(c8));
   assert(is_op(c9));
   assert(is_op(c10));
   assert(is_op(c11));
   assert(is_op(c12));
   assert(!is_op(c13));
   assert(!is_op(c14));
   assert(!is_op(c15));

   strcpy(p4->words[p4->cw], sc);
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], sc);
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], "+");
   assert(!Polish(p4));

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], "-");
   strcpy(p4->words[p4->cw+2], "+");
   strcpy(p4->words[p4->cw+3], sc);
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], "*");
   strcpy(p4->words[p4->cw+2], "?");
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Polish(p4));

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], "*");
   strcpy(p4->words[p4->cw+2], "/");
   strcpy(p4->words[p4->cw+3], sc);
   strcpy(p4->words[p4->cw+4], sc);
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], "*");
   strcpy(p4->words[p4->cw+1], "/");
   assert(!Polish(p4));

   strcpy(p4->words[p4->cw], "*");
   strcpy(p4->words[p4->cw+1], "/");
   assert(!Polish(p4));

   strcpy(p4->words[p4->cw], c2);
   strcpy(p4->words[p4->cw+1], sc);
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], c1);
   strcpy(p4->words[p4->cw+1], sc);
   assert(!Polish(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], c2);
   assert(!Polish(p4));

   strcpy(p4->words[p4->cw], "+");
   strcpy(p4->words[p4->cw+1], "+");
   strcpy(p4->words[p4->cw+2], "+");
   strcpy(p4->words[p4->cw+3], "+");
   strcpy(p4->words[p4->cw+4], ";");
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], "A");
   strcpy(p4->words[p4->cw+1], "B");
   strcpy(p4->words[p4->cw+2], "C");
   strcpy(p4->words[p4->cw+3], "D");
   strcpy(p4->words[p4->cw+4], "+");
   strcpy(p4->words[p4->cw+5], "*");
   strcpy(p4->words[p4->cw+6], "/");
   strcpy(p4->words[p4->cw+7], "0.00001");
   strcpy(p4->words[p4->cw+8], "2029391");
   strcpy(p4->words[p4->cw+9], "Z");
   strcpy(p4->words[p4->cw+10], "-");
   strcpy(p4->words[p4->cw+11], "-");
   strcpy(p4->words[p4->cw+12], ";");
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], sc);
   strcpy(p4->words[p4->cw+1], sc);
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], "/");
   strcpy(p4->words[p4->cw], sc);
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], num1);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], c4);
   strcpy(p4->words[p4->cw+3], num13);
   strcpy(p4->words[p4->cw+4], num18);
   strcpy(p4->words[p4->cw+5], sc);
   assert(Polish(p4));

   strcpy(p4->words[p4->cw], num1);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], c4);
   strcpy(p4->words[p4->cw+3], num14);
   strcpy(p4->words[p4->cw+4], num18);
   strcpy(p4->words[p4->cw+5], sc);
   assert(!Polish(p4));

   strcpy(p4->words[p4->cw], num9);
   strcpy(p4->words[p4->cw+1], c5);
   strcpy(p4->words[p4->cw+5], sc);
   assert(!Polish(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], num9);
   assert(!Polish(p4));

   p4->cw = 0;
   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], num1);
   strcpy(p4->words[p4->cw+4], c2);
   strcpy(p4->words[p4->cw+5], c4);
   strcpy(p4->words[p4->cw+6], num13);
   strcpy(p4->words[p4->cw+7], num18);
   strcpy(p4->words[p4->cw+8], sc);
   assert(Set(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "SET");
   assert(!Polish(p4));

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c3);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+4], sc);
   assert(Set(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c3);
   strcpy(p4->words[p4->cw+2], sign);
   assert(!Set(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], c3);
   strcpy(p4->words[p4->cw+1], sign);
   strcpy(p4->words[p4->cw+2], "/");
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Set(p4));
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
   assert(!Set(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], c2);
   strcpy(p4->words[p4->cw+1], sc);
   assert(!Set(p4));
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
   assert(Set(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c6);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Set(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], "Set");
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], sc);
   assert(!Set(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], set);
   strcpy(p4->words[p4->cw+1], c4);
   strcpy(p4->words[p4->cw+2], sign);
   strcpy(p4->words[p4->cw+3], "+");
   assert(!Set(p4));
   p4->cw++;

   /*//////*/
   strcpy(p5->words[p5->cw], "DO");
   strcpy(p5->words[p5->cw+1], "A");
   assert(!Do(p5));
   p5->cw++;
   assert(!Do(p5));
   p5->cw++;
   assert(!Do(p5));
   p5->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], cb);
   assert(Do(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], c2);
   strcpy(p4->words[p4->cw+1], from);
   strcpy(p4->words[p4->cw+2], num3);
   strcpy(p4->words[p4->cw+3], to);
   strcpy(p4->words[p4->cw+4], num1);
   strcpy(p4->words[p4->cw+5], ob);
   strcpy(p4->words[p4->cw+6], cb);
   assert(!Do(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], cb);
   assert(Do(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], from);
   strcpy(p4->words[p4->cw+2], num3);
   strcpy(p4->words[p4->cw+3], to);
   strcpy(p4->words[p4->cw+4], num1);
   strcpy(p4->words[p4->cw+5], ob);
   strcpy(p4->words[p4->cw+6], cb);
   assert(!Do(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], to);
   strcpy(p4->words[p4->cw+4], num3);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], cb);
   assert(!Do(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c1);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], cb);
   assert(!Do(p4));
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
   assert(!Do(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], rt);
   strcpy(p4->words[p4->cw+10], num10);
   strcpy(p4->words[p4->cw+11], rt);
   strcpy(p4->words[p4->cw+12], num13);
   strcpy(p4->words[p4->cw+13], lt);
   strcpy(p4->words[p4->cw+14], num11);
   assert(!Do(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], rt);
   strcpy(p4->words[p4->cw+10], num10);
   strcpy(p4->words[p4->cw+11], rt);
   strcpy(p4->words[p4->cw+12], num13);
   strcpy(p4->words[p4->cw+13], lt);
   strcpy(p4->words[p4->cw+14], num11);
   strcpy(p4->words[p4->cw+15], cb);
   assert(Do(p4));
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
   assert(Instruction(p4));
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
   assert(Instrctlst(p4));
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
   assert(!Instrctlst(p4));
   p4->cw++;

   strcpy(p4->words[p4->cw], do_lbl);
   strcpy(p4->words[p4->cw+1], c2);
   strcpy(p4->words[p4->cw+2], from);
   strcpy(p4->words[p4->cw+3], num3);
   strcpy(p4->words[p4->cw+4], to);
   strcpy(p4->words[p4->cw+5], num1);
   strcpy(p4->words[p4->cw+6], ob);
   strcpy(p4->words[p4->cw+7], fd);
   strcpy(p4->words[p4->cw+8], num9);
   strcpy(p4->words[p4->cw+9], rt);
   strcpy(p4->words[p4->cw+10], num10);
   strcpy(p4->words[p4->cw+11], rt);
   strcpy(p4->words[p4->cw+12], num13);
   strcpy(p4->words[p4->cw+13], lt);
   strcpy(p4->words[p4->cw+14], num11);
   strcpy(p4->words[p4->cw+15], cb);
   assert(Instruction(p4));
   p4->cw++;

   /* Examples from Question Description */
   /* Example 1 */
   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], fd);
   strcpy(p4->words[p4->cw+2], "30");
   strcpy(p4->words[p4->cw+3], lt);
   strcpy(p4->words[p4->cw+4], "45");
   strcpy(p4->words[p4->cw+5], fd);
   strcpy(p4->words[p4->cw+6], "30");
   strcpy(p4->words[p4->cw+7], lt);
   strcpy(p4->words[p4->cw+8], "45");
   strcpy(p4->words[p4->cw+9], fd);
   strcpy(p4->words[p4->cw+10], "30");
   strcpy(p4->words[p4->cw+11], lt);
   strcpy(p4->words[p4->cw+12], "45");
   strcpy(p4->words[p4->cw+13], fd);
   strcpy(p4->words[p4->cw+14], "30");
   strcpy(p4->words[p4->cw+15], lt);
   strcpy(p4->words[p4->cw+16], "45");
   strcpy(p4->words[p4->cw+17], fd);
   strcpy(p4->words[p4->cw+18], "30");
   strcpy(p4->words[p4->cw+19], lt);
   strcpy(p4->words[p4->cw+20], "45");
   strcpy(p4->words[p4->cw+21], fd);
   strcpy(p4->words[p4->cw+22], "30");
   strcpy(p4->words[p4->cw+23], lt);
   strcpy(p4->words[p4->cw+24], "45");
   strcpy(p4->words[p4->cw+25], fd);
   strcpy(p4->words[p4->cw+26], "30");
   strcpy(p4->words[p4->cw+27], lt);
   strcpy(p4->words[p4->cw+28], "45");
   strcpy(p4->words[p4->cw+29], fd);
   strcpy(p4->words[p4->cw+30], "30");
   strcpy(p4->words[p4->cw+31], lt);
   strcpy(p4->words[p4->cw+32], "45");
   strcpy(p4->words[p4->cw+33], cb);
   assert(Main_gram(p4));
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
   strcpy(p4->words[p4->cw+8], fd);
   strcpy(p4->words[p4->cw+9], "30");
   strcpy(p4->words[p4->cw+10], lt);
   strcpy(p4->words[p4->cw+11], "45");
   strcpy(p4->words[p4->cw+12], cb);
   strcpy(p4->words[p4->cw+13], cb);
   assert(Main_gram(p4));
   p4->cw++;

   /* Using Variables Example */
   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], do_lbl);
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], from);
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], to);
   strcpy(p4->words[p4->cw+6], "100");
   strcpy(p4->words[p4->cw+7], ob);
   strcpy(p4->words[p4->cw+8], set);
   strcpy(p4->words[p4->cw+9], "C");
   strcpy(p4->words[p4->cw+10], sign);
   strcpy(p4->words[p4->cw+11], "A");
   strcpy(p4->words[p4->cw+12], "1.5");
   strcpy(p4->words[p4->cw+13], "*");
   strcpy(p4->words[p4->cw+14], sc);
   strcpy(p4->words[p4->cw+15], fd);
   strcpy(p4->words[p4->cw+16], "C");
   strcpy(p4->words[p4->cw+17], rt);
   strcpy(p4->words[p4->cw+18], "62");
   strcpy(p4->words[p4->cw+19], cb);
   strcpy(p4->words[p4->cw+20], cb);
   assert(Main_gram(p4));
   p4->cw++;

   /* Nested Loops Example */
   strcpy(p4->words[p4->cw], ob);
   strcpy(p4->words[p4->cw+1], do_lbl);
   strcpy(p4->words[p4->cw+2], "A");
   strcpy(p4->words[p4->cw+3], from);
   strcpy(p4->words[p4->cw+4], "1");
   strcpy(p4->words[p4->cw+5], to);
   strcpy(p4->words[p4->cw+6], "50");
   strcpy(p4->words[p4->cw+7], ob);
   strcpy(p4->words[p4->cw+8], fd);
   strcpy(p4->words[p4->cw+9], "A");
   strcpy(p4->words[p4->cw+10], rt);
   strcpy(p4->words[p4->cw+11], "30");
   strcpy(p4->words[p4->cw+12], do_lbl);
   strcpy(p4->words[p4->cw+13], "B");
   strcpy(p4->words[p4->cw+14], from);
   strcpy(p4->words[p4->cw+15], "1");
   strcpy(p4->words[p4->cw+16], to);
   strcpy(p4->words[p4->cw+17], "8");
   strcpy(p4->words[p4->cw+18], ob);
   strcpy(p4->words[p4->cw+19], set);
   strcpy(p4->words[p4->cw+20], "C");
   strcpy(p4->words[p4->cw+21], sign);
   strcpy(p4->words[p4->cw+22], "A");
   strcpy(p4->words[p4->cw+23], "5");
   strcpy(p4->words[p4->cw+24], "/");
   strcpy(p4->words[p4->cw+25], sc);
   strcpy(p4->words[p4->cw+26], fd);
   strcpy(p4->words[p4->cw+27], "C");
   strcpy(p4->words[p4->cw+28], rt);
   strcpy(p4->words[p4->cw+29], "45");
   strcpy(p4->words[p4->cw+30], cb);
   strcpy(p4->words[p4->cw+31], cb);
   strcpy(p4->words[p4->cw+32], cb);
   assert(Main_gram(p4));
   p4->cw++;

   strcpy(p5->words[p5->cw], ob);
   assert(!Main_gram(p5));

   free_all(p2);
   free_all(p3);
   free_all(p4);
   free_all(p5);
}
