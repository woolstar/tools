	//// generalstringlist
	//	keep strings tucked away
	//	accessable by array

#include <general.h>
#include	"generalstringlist.h"
#include	"generaloutput.h"

	// constructor
generalstringlist::generalstringlist(unsigned int asize)
{
	int itmp ;

	if (asize < 256) asize= 256 ;

	muse= 0, mmax= 0, mcount= 0, mcountmax= 0;

	mbuffer= (char *) malloc(asize * sizeof(char)) ;
	if (NULL == mbuffer)
		return ;

	itmp= asize / 8 ;
	if (itmp < 64) itmp= 64 ;
	moffset= (unsigned int *) malloc(itmp * sizeof(unsigned int)) ;
	if (NULL == moffset)
		return ;

	mmax= asize, mcountmax= itmp ;
}

generalstringlist::generalstringlist(const generalstringlist & aref)
{
	muse= 0, mmax= 0, mcount= 0, mcountmax= 0;
	copy(aref) ;
}

void	generalstringlist::copy(const generalstringlist & aref)
{
	unsigned int	isize, ispan ;

	isize= (aref.muse + 511) & ~ 255;
	ispan= (aref.mcountmax + 255) & ~ 63 ;
	muse= 0, mcount= 0;

	if (isize > mmax)
	{
		free(mbuffer) ;
		mmax= 0 ;
		mbuffer= (char *) malloc(isize * sizeof(char)) ;
		if (NULL == mbuffer)
			return ;
	}
	if (ispan > mcountmax)
	{
		free(moffset) ;
		mcountmax= 0;
		moffset= (unsigned int *) malloc(ispan * sizeof(unsigned int)) ;
		if (NULL == moffset)
			return ;
	}

	memcpy(mbuffer, aref.mbuffer, aref.muse * sizeof(char)) ;
	memcpy(moffset, aref.moffset, aref.mcount * sizeof(unsigned int)) ;

	muse= aref.muse, mcount= aref.mcount ;
	mmax= isize, mcountmax= ispan ;
}

generalstringlist:: ~ generalstringlist()
{
	if (mmax && mbuffer) free(mbuffer) ;
	if (mcountmax && moffset) free(moffset) ;
}

	// item operations
void	generalstringlist::add(const char * astring)
{
	int	ilen ;

	ASSERTPOINTER(astring) ;
	ASSERT(* astring) ;

	ilen= strlen(astring) +1 ;
		// resize if buffer too small
	if ((ilen + muse) > mmax)
	{
		int ibump ;
		if (ilen > 256) ibump= (2 * ilen) & ~ 3 ; else ibump= 256 ;
		if (! resize(mmax + ibump))
			return ;
	}
		// resize index if full
	if ((mcount == mcountmax) && ! resize_offset(mcountmax + 64))
		return ;

	moffset[mcount ++]= muse ;
	memcpy(mbuffer + muse, astring, ilen) ;
	muse += ilen ;
}

void	generalstringlist::remove(unsigned int aindex)
{
	ASSERT((aindex >= 0) && (aindex < mcount)) ;

	mcount -- ;
	if (aindex == mcount)
	{
			// take top string off of usage count
		muse= moffset[mcount] ;
	}
	else
	{
		int isize = moffset[aindex +1] - moffset[aindex] ;
		int irem;
		unsigned int istep ;
		unsigned int	* ofstep ;

		// always move strings over to preserve consistancy

			// calculate tail size
		irem= muse - moffset[aindex +1] ;
			// move tail string over
		memmove(mbuffer + moffset[aindex], mbuffer + moffset[aindex +1], irem * sizeof(char)) ;
			// change tail offsets
		for (istep= mcount - aindex, ofstep= moffset +aindex +1; (istep --); ofstep ++)
			* ofstep -= isize ;
			// change global usage
		muse -= isize ;

			// move offsets over
		memmove(& moffset[aindex], & moffset[aindex +1], (mcount - aindex) * sizeof(unsigned int)) ;
	}
}

	// retrieve entire list
char *	generalstringlist::tostring(char asep) const
{
	char * sdest, * stmp ;

	if (! muse || ! mcount) return NULL;

	stmp= (char *) malloc(muse * sizeof(char)) ;
	if (NULL == stmp) return NULL;

	memcpy(stmp, mbuffer, muse * sizeof(char)) ;
	sdest= stmp ;

	if (asep)
	{
		int	istep ;
		unsigned int	* ofstep ;

		for (istep= mcount -1, ofstep= moffset +1; (istep --); ofstep ++)
			stmp[*ofstep -1]= asep;

		stmp[muse-1]= asep ;
	}

	return sdest ;
}

	// retrieve entire list formatted
char *	generalstringlist::tostring(const char * afmt) const
{
	char * sdest, * stmp ;
	int	istep ;
	unsigned int	* ofstep ;
	unsigned long lsize, llen ;

	if (! muse || ! mcount) return NULL;
	llen= strlen(afmt) ;
	ASSERT(llen >= 2) ;

	stmp= strstr(const_cast<char*>(afmt), "%s") ;
	if (! stmp) return NULL ;

	llen -=2 ;
	lsize= muse + (llen - 1) * mcount +1 ;
	sdest= (char *) malloc(lsize * sizeof(char)) ;
	if (NULL == sdest) return NULL ;

	for (istep= mcount, ofstep= moffset, stmp= sdest; (istep --); ofstep ++)
	{
		char * ssrc= mbuffer + * ofstep ;
		if (! * ssrc) continue ;
		llen= sprintf(stmp, afmt, ssrc) ;
		stmp += llen ;
	}

	return sdest ;
}

	// retrieve entire list joined with strings
char *	generalstringlist::join(const char * asep) const
{
	char * sdest, * stmp ;
	int	istep ;
	unsigned int	* ofstep ;
	unsigned long lsize, llen ;
	bool past= false ;

	ASSERTPOINTER(asep) ;

	if (! muse || ! mcount) return NULL ;

	llen= strlen(asep) ;
	ASSERT(llen > 0) ;

	if (1 == mcount)
		return strdup(mbuffer) ;

	lsize= muse + llen * (mcount -1) +1 ;
	if (lsize <= mcount) return NULL;
	lsize -= mcount ;

	sdest= (char *) malloc(lsize * sizeof(char)) ;
	if (NULL == sdest) return NULL ;

	for (istep= mcount, ofstep= moffset, stmp= sdest; (istep --); ofstep ++)
	{
		unsigned int ulen= strlen(mbuffer + *ofstep) ;
		if (! ulen) continue ;
		if (past) { memcpy(stmp, asep, llen * sizeof(char)) ;  stmp += llen; }
		memcpy(stmp, mbuffer + * ofstep, ulen * sizeof(char)) ;
		stmp += ulen ;
		past= true ;
	}
	* stmp= '\0' ;

	return sdest ;
}

	// output to file
void	generalstringlist::print(FILE * fdest, const char * aformat) const
{
	int istep ;
	unsigned int * ofstep;

	ASSERTPOINTER(fdest) ;
	for (istep= mcount, ofstep= moffset; (istep --); ofstep ++)
		{ if (* (mbuffer + * ofstep)) fprintf(fdest, aformat, mbuffer + * ofstep) ; }
}

void	generalstringlist::print(generaloutput * adest, const char * aformat) const
{
	int istep ;
	unsigned int * ofstep;

	ASSERTPOINTER(adest) ;
	for (istep= mcount, ofstep= moffset; (istep --); ofstep ++)
		{ if (* (mbuffer + * ofstep)) adest-> printf(aformat, mbuffer + * ofstep) ; }
}

	// retrieve entry
const char *	generalstringlist::operator[](int aindex) const
{
		// bounds check
	if ((aindex < 0) || ((unsigned) aindex >= mcount)) return NULL ;

	return mbuffer + moffset[aindex] ;
}

	//// memory management

	// resize string buffer
bool	generalstringlist::resize(unsigned int asize)
{
	char	* tmpptr ;
	if (asize <= mmax) return true ;

	tmpptr= (char *) realloc(mbuffer, asize * sizeof(char)) ;
	if (tmpptr)
	{
		mbuffer= tmpptr, mmax= asize ;
		return true ;
	}
		else return false ;
}

	// resize offset array
bool	generalstringlist::resize_offset(unsigned int asize)
{
	unsigned int	* tmpptr ;
	if (asize <= mcountmax) return true ;

	tmpptr= (unsigned int *) realloc(moffset, asize * sizeof(unsigned int)) ;
	if (tmpptr)
	{
		moffset= tmpptr, mcountmax= asize ;
		return true ;
	}
		else return false ;
}
