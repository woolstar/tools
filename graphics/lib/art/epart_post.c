#include "basic.h"
#include "tscene.h"
#include "epart_post.h"

	/** epart_word : file hack */
extern FILE * getFile(void) ;

int	flush_block(void)
{
	FILE *f= getFile(); 
	char strline[256], * p, c ;

	if (NULL == f)
		return -1 ;

	while (fgets(strline, sizeof(strline), f))
	{
		sM_start(p, strline) ;
		if ('}' == c) return 0;
		/* if (('#' == c) || ! c) continue ; */
	}

	return 0;
}

int	post_read(char * dest, int len)
{
	FILE *f= getFile(); 
	char strline[256], * p, * q, c ;
	int irem, ilen ;

	if (NULL == f)
		return -1 ;

	*dest= '\0' ;
	irem= len ;
	while ((irem > 1) && fgets(strline, sizeof(strline), f))
	{
		sM_start(p, strline) ;
		if (('#' == c) || ! c) continue ;
		if ('}' == c) return 0;
		P_zEOL
		strcat(p, "\n") ;
		ilen= strlen(p);
		if (ilen > irem)
			{ ilen= irem -1, p[ilen-1]= '\0' ; }
		strcat(dest, p);
		irem -= ilen ;
	}

	return 0;
}

int	post_write(tss_exec * ptr, FILE * fdest)
{
	char * p, * q, strline[256], c ;

	fprintf(fdest, "\tpost %s\n", ptr-> cmdword);

	p= ptr-> cmdform ;
	if (! * p) return 0;

	fprintf(fdest, "\t{\n") ;
	while (* p)
	{
		for (q= strline; ((c= *(p ++)) && ('\n' != c)); q ++)
			{ *q= c; }
		*q= '\0' ;
		fprintf(fdest, "\t\t%s\n", strline);
	}

	fprintf(fdest, "\t}\n") ;
	return 0;
}

