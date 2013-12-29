#include <general.h>

#include <ctype.h>

void	generalstring::ncopy(char * adest, const char * asource, unsigned long asize)
{
	char c;

		// test for fatal parameter violations
		// no destination, no source, no space
	ASSERTPOINTERMESSAGE(adest, "Null destination to generalstring::ncopy") ;
	ASSERTPOINTERMESSAGE(asource, "Null source to generalstring::ncopy") ;
	ASSERTMESSAGE((asize > 0), "Zero size to generalstring::ncopy") ;

		// while room in adest, copy from aSource
	while ((asize > 1) && (c= *(asource ++)))
	{
		*(adest ++)= c;
		asize -- ;
	}

		// terminate
	* adest= '\0'; 
}

	// ncaptureword
void	generalstring::ncaptureword(char * adest, const char * & asource, unsigned long asize, char asep)
{
	char c ;

		// while in the word
	while ((c= * asource) && ! iswhite(c) && (asep != c))
	{
			// copy if there's room
		if (asize > 1) { *(adest ++)= c, asize --; }
		asource ++ ;
	}

	ASSERT((asize > 0)) ;
	* adest= '\0' ;

		// skip optional seperator if thats what we hit
	if (asep && (asep == * asource)) asource ++ ;
		// skip white
	while ((c= *asource) && iswhite(c)) asource ++ ;
}

void	generalstring::ncaptureparam(char * adest, const char * asource, unsigned long asize)
{
	char c, * pad;

	pad= NULL ;
	while (c= * asource)
	{
		if (iswhite(c)) { if (! pad) pad= adest; } else pad= NULL ;
		if (asize > 1) { *(adest ++)= c; }
			else break ;
		asize --, asource ++ ;
	}

	if (pad) *pad= '\0' ; else * adest= '\0' ;
	return ;
}

void	generalstring::toupper(char * astring)
{
	char c ;
	while (c= * astring) { if (islower(c)) * astring= ::toupper(c) ;  astring ++; }
}
	// 
void	generalstring::tolower(char * astring)
{
	char c ;
	while (c= * astring) { if (isupper(c)) * astring= ::tolower(c) ;  astring ++; }
}

	// trim off all trailing whitespace
void	generalstring::trim_space(char * astring)
{
	char	* p;

		// find end
	for (p= astring; (* p); p ++) ;

		// examine backwards
	while (p > astring)
	{
		if (! iswhite(p[-1])) break ;
		*(-- p)= '\0' ;
	}
}

void	generalstring::sequence(char * adest, const char * asource, unsigned int anum)
{
	const char * p ;
	char c, *q, *r, tmpstr[16], tmpfmt[16];
	int rl, roff ;

	p= asource, q= adest ;
	while (c= *(p ++))
	{
		if ('*' != c) { *(q ++)= c;  continue; }

		roff= atoi(p), rl= 0;
		if (! roff) roff= 1;
		while (isdigit(* p)) { p ++, rl ++; }

		if (rl > 1) sprintf(tmpfmt, "%%0%dd", rl);
			else strcpy(tmpfmt, "%d");
		sprintf(tmpstr, tmpfmt, anum + roff -1);
		r= tmpstr ;
		while (c= *(r ++)) *(q ++)= c;
	}

	*q= '\0';
}

