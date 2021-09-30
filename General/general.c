#include "general.h"

bool Varnum(char *c)
{
   if(is_num(c) || is_var(c)){
      return true;
   }
   return false;
}

bool is_var(char* c)
{
   int x;
   if((int)strlen(c) > 1){
      return false;
   }
   x = (int)c[0];
   if((x >= 65) && (x <= 90)){
      return true;
   }
   return false;
}

unsigned int word_count(char *textfile)
{
   FILE *fp = nfopen(textfile, "r");
   char tmp_wd[MAXTOKENLEN];
   int i = 0;
   while(fscanf(fp, "%s", tmp_wd) == 1){
      i++;
   }
   if(!i){
      on_error("Error: Empty File.");
   }
   fclose(fp);
   return i;
}

bool is_op(char* c)
{
   if((int)strlen(c) > 1){
      return false;
   }
   if(c[0] == '+' || c[0] == '-' || c[0] == '*' || c[0] == '/'){
      return true;
   }
   return false;
}

bool is_num(char* c)
{
   int i, point_cnt = 0, neg_cnt = 0;
   for(i = 0; i < (int)strlen(c); i++){
      if(!isdigit(c[i])){
         switch(c[i]) {
            case '.':
               point_cnt++;
               if(!i){
                  return false;
               }
               break;
            case '-':
               neg_cnt++;
               if(i || !isdigit(c[i+1])){
                  return false;
               }
               break;
            default:
               return false;
         }
      }
   }
   if(point_cnt > 1 || neg_cnt > 1){
      return false;
   }
   return true;
}
