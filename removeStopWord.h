#ifndef __STOPWORD__
#define __STOPWORD__
#include  <string.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <assert.h>
#include  <locale.h>
#include  <memory.h>
#define    ISSPACE(x)      ((x)==-95||(x)==' '||(x)=='\r'||(x)=='\n'||(x)=='\f'||(x)=='\b'||(x)=='\t')
/*
* @auther LiYuqian IDC HUST WuHan
* @email  yuqianfly@gmail.com
*/

void  Trim( char *String )
{
      char  *Tail, *Head;
      for ( Tail = String + strlen( String ) - 1; Tail >= String; Tail -- )
	  {   //printf("%d\n",*Tail);
		  if ( !ISSPACE( *Tail ) )
                        break;
	  } 
	  Tail[1] = '\0';
        for ( Head = String; Head <= Tail; Head ++ )
              if ( !ISSPACE( *Head ) )
                        break;
        if ( Head != String )
              memcpy(String,Head,(Tail-Head + 1) * sizeof(char));
		
        //return String;
}
/*
char *trim(char *str)
{
        char *p = str;
        char *p1;
        if(p)
        {
                p1 = p + strlen(str) - 1;
                while(*p && isspace(*p)) p++;
                while(p1 > p && isspace(*p1)) *p1-- = '\0';
        }
        return p;
}
*/

/*
int main(int argc,char* argv[]){
   char  testchar[]="    　　　　　　　　　　　北京市行政措施备案规定    　　　　　　　　　　　";
   AllTrim(testchar);
   printf(":%s",testchar);
   printf("test\n");
   char * newchar=trim(testchar);
   printf(":%s",newchar);
   printf("test\n");

   Trim(testchar);
   printf(":%s",testchar);
   printf("test\n");

   return 0;	
}*/
#endif
