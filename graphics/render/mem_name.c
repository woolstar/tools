#include "basic.h"

char *mem_name(n)
long n;
{
   static char stwing[16];

   if (n > 104857599) 
      { sprintf(stwing, "%ld Meg", n / 1048576);  return stwing; }
   if (n > 8388607) 
      { sprintf(stwing, "%1.1f Meg", n / 1048576.0);  return stwing; }
   if (n > 102399) { sprintf(stwing, "%ldK", n / 1024);  return stwing; }
   if (n > 10239) { sprintf(stwing, "%1.1fK", n / 1024.0);  return stwing; }
   sprintf(stwing, "%d bytes", (int) n);  
   return stwing;
}

