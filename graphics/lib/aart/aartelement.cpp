#include "aar.h"

#include	<ctype.h>

#include	"aart_element.h"
#include	"aart_link.h"

aart_element::aart_element(const char * astr, aart_component * aparent) : aart_component(aparent)
{
	generalstring::captureKnown(mname, astr) ;

	mlink= NULL ;
}

aart_element:: ~ aart_element()
{
	if (mlink)
	{
		mlink->mjoin= NULL ;
		delete mlink ;
	}
}

bool	aart_element::parsevec(vector & aparam, const char * astr)
{
	float fa, fb, fc ;

	generalstring::skip_word(astr) ;
	fa= (float) atof(astr) ;
	generalstring::skip_word(astr) ;
	fb= (float) atof(astr) ;
	generalstring::skip_word(astr) ;
	if (! * astr)
		return false ;

	fc= (float) atof(astr) ;
	aparam.x= fa, aparam.y= fb, aparam.z= fc ;
	return true ;
}

bool	aart_element::parseq(quaternion & aparam, const char * astr)
{
	float fa, fb, fc, fd ;

	generalstring::skip_word(astr) ;
	fa= (float) atof(astr) ;
	generalstring::skip_word(astr) ;
	fb= (float) atof(astr) ;
	generalstring::skip_word(astr) ;
	fc= (float) atof(astr) ;
	generalstring::skip_word(astr) ;
	if (! * astr)
		return false ;

	fd= (float) atof(astr) ;

	if ((fa < -1.000001) || (fa > 1.000001)
			|| (fb < -1.000001) || (fb > 1.000001)
			|| (fc < -1.000001) || (fc > 1.000001)
			|| (fd < -1.000001) || (fd > 1.000001))
		return false ;

	aparam.s= fa, aparam.i= fb, aparam.j= fc, aparam.k= fd ;
	return true ;
}

bool	aart_element::parsecolor(color & aparam, const char * astr)
{
	float fa, fb, fc ;

	if (! isdigit(* astr))
		generalstring::skip(astr) ;

	fa= (float) atof(astr) ;
	generalstring::skip(astr) ;

	if (* astr)
	{
		fb= (float) atof(astr) ;
		generalstring::skip(astr) ;
		if (! * astr)
			return false ;
		fc= (float) atof(astr) ;
	}
		else fb= fc= fa ;

	aparam.r= fa, aparam.g= fb, aparam.b= fc ;
	return true ;
}

bool	aart_element::parseval(float & aval, const char * astr)
{
	char c ;

	generalstring::skip_word(astr) ;
	c= * astr ;

	if (isdigit(c) || ('-' == c))
	{
		aval= (float) atof(astr) ;
		return true ;
	}

	return false ;
}

bool	aart_element::parsepoint(int & ax, int & ay, const char * astr)
{
	char c ;

	generalstring::skip_word(astr) ;
	c= * astr ;
	if (! isdigit(c) && ('-' != c))
		return false ;
	ax= atoi(astr) ;

	generalstring::skip_word(astr) ;
	c= * astr ;
	if (! isdigit(c) && ('-' != c))
		return false ;
	ay= atoi(astr) ;

	return true ;
}