#include "aar.h"
#include	"aart_output.h"

#include	<stdarg.h>

static char	strtab[]= "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t" ;
#define	MAX_TAB	32

aart_output::aart_output(const char * adest) : generalfileoutput(adest)
{
	indent= 0 ;
	mtab= strtab +MAX_TAB ;
}

	///

bool	aart_output::print(const char * astr)
{
	char c ;

	if (NULL == fdest) return false ;

	c= * astr ;
	if ('}' == c) shift_out() ;
	if (indent) fputs(mtab, fdest) ;
	fputs(astr, fdest) ;

	if ('{' == c) shift_in() ;
	return true ;
}

bool	aart_output::printf(const char * amsg, ...)
{
	va_list	 vl ;

	if (fdest)
	{
		char c= * amsg ;
		va_start(vl, amsg) ;

		if ('}' == c) shift_out() ;
		if (indent) fputs(mtab, fdest) ;
		vfprintf(fdest, amsg, vl) ;
		va_end(vl) ;

		if ('{' == c) shift_in() ;
	}
		else return false ;

	return true ;
}

	///

void	aart_output::shift_in(void)
{
	indent ++ ;
	if (mtab > strtab) mtab -- ;
}

void	aart_output::shift_out(void)
{
	if (indent > 0)
	{
		indent -- ;
		if (indent < MAX_TAB)
			mtab ++ ;
	}
}