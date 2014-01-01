#include "ye.h"

#include	"rqdata.h"

static unsigned int		t ;
static const char	* path = NULL ;

extern "C"
{
	FILE * cto_fileopen(char *) ;
} ;

void	cto_setup(int atime, const char * apath) { t= atime, path= apath; }

FILE	*cto_fileopen(char *aname)
{
	FILE * ftmp ;
	char	tmpname[512] ;

	if (strchr(aname, '*'))
	{
		generalstring::sequence(tmpname, aname, t) ;
		aname= tmpname ;
	}

	ftmp= fopen(aname, "rb") ;
	if ((NULL == ftmp) && path)
	{
		char tmpstr[1024] ;
		sprintf(tmpstr, "%s/%s", path, aname) ;
		ftmp= fopen(tmpstr, "rb") ;
	}

	return ftmp ;
}
