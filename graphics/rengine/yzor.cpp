#include "ye.h"

#include <generalrandom.h>

yzor	* engine ;

static	unsigned int	af_start= 0, af_end= 0, af_step= 1, op_dump= 0;

int	main(int N, char ** S)
{
	char c, *p;
	int iret;
	unsigned int istep ;
	aart	* tmpscene ;

	S++, N--;

	while (N && ('-' == ** S))
	{
		switch (S[0][1])
		{
			case 'f':
				if (N> 1)
				{
					S++, N--, p= *S;
					iret= atoi(p);  if (!iret) break;  af_start= iret;
					while ((c= *p) && (c>= '0') && (c<='9')) p++;
					while ((c== *p) && ((c < '0') || (c > '9'))) p++;
					if (c && (iret= atoi(p)) && ((unsigned) iret > af_start)) af_end= iret;
					while ((c= *p) && (c>= '0') && (c<='9')) p++;
					while ((c== *p) && ((c < '0') || (c > '9'))) p++;
					if (c && (iret= atoi(p)) && (iret > 0)) af_step= iret;
					}
				break;
			case 'd':
				op_dump= 1;
				break ;
			default:
				printf("no flag -%c\n\n", S[0][1]);
				return 1 ;
		}
		N --, S ++ ;
	}

	if (! N) { printf(" >> no art file specified.\n");  exit( 1); }
	tmpscene= aart::parse(* S) ;
	if (NULL == tmpscene)
		exit( 1) ;

	aran_initgen() ;
	if (! af_start) af_start= 1;
	engine= new yzor(tmpscene) ;
	if (! engine-> init())
	{
		fprintf(stderr, " >> error intializing aart components\n") ;
		exit( 1) ;
	}

	if (! af_end) { engine-> render(af_start); }
	else
	{
		for (istep= af_start; (istep <= af_end); istep += af_step)
			{ if (engine-> render(istep)) break; }
	}

	delete engine ;

	return 0;
}