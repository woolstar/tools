#include <general.h>

#include "aart.h"

main(int N, char ** S)
{
	aart	* global ;

	N --, S ++ ;

	if (N)
	{
		global= aart::parse(* S) ;
		if (NULL == global)
		{
			perror("failed") ;
			exit( 1) ;
		}
		N --, S ++ ;
	}

	if (N && global)
	{
		printf("output ...\n") ;
		global-> print(* S) ;
	}

	return 0;
}
