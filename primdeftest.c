/*

    	Test primitive definition in Atlast
	
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "pezdef.h"

prim ptime()
{
	So(1);
	Push = time(NULL);
}

prim phhmmss()
{
	struct tm *lt;

	Sl(1);
	So(2);
	lt = localtime(&S0);
	S0 = lt->tm_hour;
	Push = lt->tm_min;
	Push = lt->tm_sec;
}

static struct primfcn timep[] = {
	{"0TIME", ptime},
	{"0HHMMSS", phhmmss},
	{NULL, (codeptr) 0}
};

int main()
{
	char t[132];
	pez_init();
	pez_primdef(timep);
	while(printf("-> "), fgets(t, 132, stdin) != NULL)
		pez_eval(t);
	return 0;
}
