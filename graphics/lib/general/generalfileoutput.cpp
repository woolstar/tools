#include	"general.h"
#include	"generaloutput.h"

#include	<stdarg.h>

generalfileoutput::generalfileoutput()
{
	fdest= NULL ;
}

generalfileoutput::generalfileoutput(const char * afile)
{
	fdest= fopen(afile, "w") ;
}

generalfileoutput:: ~ generalfileoutput()
{
	if (fdest) fclose(fdest) ;
}

	//

bool	generalfileoutput::print(const char * astr)
{
	if (! isvalid()) return false ;

	fputs(astr, fdest) ;
	return true ;
}

bool	generalfileoutput::printf(const char * amsg, ...)
{
	va_list	 vl ;

	if (! isvalid()) return false ;

	va_start(vl, amsg) ;
	vfprintf(fdest, amsg, vl) ;
	va_end(vl) ;

	return true ;
}