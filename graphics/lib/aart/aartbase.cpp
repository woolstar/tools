#include "aar.h"
#include "aart_gen.h"
#include "aart_output.h"

aart_base *	aart_base::parse(const char * afile)
{
	aart_base * base= NULL ;
	FILE * fsrc ;

	fsrc= fopen(afile, "r") ;
	if (NULL == fsrc)
		return NULL ;

	base= new aart_base() ;

	base-> load(fsrc) ;
	return base ;
}

void	aart_base::load(FILE * fsrc)
{
	char	tmpstr[1024], * p ;
	int		ignorelevel ;
	unsigned long	linenum ;
	bool	founderror ;
	generalstringlist  * ignoredest ;
	aart_component * cur, * ptr ;
	int		paramch ;

	cur= NULL, ptr= NULL ;
	ignorelevel= 0, ignoredest= & other ;
	paramch= 0 ;
	linenum= 0 ;
	founderror= false ;

	aart_gen::init() ;

	while (fgets(tmpstr, sizeof(tmpstr), fsrc))
	{
		p= tmpstr ;
		generalstring::skip_space(p) ;
		linenum ++ ;

		if (! * p)
			continue ;

		if ('{' == * p)
		{
				// opening a group
			if (ignorelevel)
			{
					// deeper ignoring
				ignorelevel ++ ;
				generalstring::trim_space(p) ;
				ignoredest-> add(p) ;
				continue ;
			}

			p ++ ;
			generalstring::skip_space(p ) ;

				// look for meaning
			ptr= find(p, cur) ;
			if (ptr)
			{
				if (1 == (long) ptr)
				{
					ignoredest-> add(p -1) ;
				}
				else
				if (2 != (long) ptr)
				{
					cur= ptr ;
					ignoredest= & (cur-> other) ;
				}
					else paramch ++ ;
			}
			else
			{
					// start ignoring
				ignorelevel ++ ;
				generalstring::trim_space(p) ;
				ignoredest-> add(p -1) ;
			}
		}
		else
		if ('}' == * p)
		{
				// closing a group
			if (ignorelevel)
			{
				ignorelevel -- ;
				generalstring::trim_space(p) ;
				ignoredest-> add(p) ;
			}
			else
			{
				if (cur)
				{
					cur-> leavescope() ;
					cur= cur-> parent ;
				}
				else
				{
					if (paramch) paramch -- ;
						else ignoredest-> add(p) ;
				}

					// realign ignore dest
				if (cur) ignoredest= & (cur-> other) ;
					else ignoredest= & other ;
			}
		}
		else
		{
			generalstring::trim_space(p) ;
			if (ignorelevel)
			{
				ignoredest-> add(p) ;
			}
			else
			if (cur)
			{
				if (! cur-> parse(p, fsrc))
				{
					if (! founderror)
					{
						char msg[256] ;

						sprintf(msg, "error in parsing at line %d", linenum) ;
						generallog::message("error in parsing", GL::eWarning) ;
						founderror= true ;
					}

					cur= cur-> parent ;

					if (cur) ignoredest= & (cur-> other) ;
						else ignoredest= & other ;
				}
			}
			else
			{
				ignore(p) ;
			}
		}
	}

	aart_gen::close() ;
	fclose(fsrc) ;
}

void	aart_base::add(aart_component * acomp)
{
	contains.append(acomp) ;
}

void	aart_base::ignore(const char * astring)
{
	other.add(astring) ;
}

bool	aart_base::print(const char * adest)
{
	aart_output * aodest= new aart_output(adest) ;
	bool bres ;

	if (! aodest-> isvalid())
	{
		delete aodest ;
		return false ;
	}

	bres= print(aodest) ;
	delete aodest ;

	return bres ;
}

bool	aart_base::print(aart_output * adest)
{
	tbagstepper<aart_component *> * step ;
	aart_component * ptr ;
	bool bres ;

	bres= true ;
	step= new tbagstepper<aart_component *>(& contains) ;
	while (step-> next(ptr))
		{ if (! ptr->print(adest)) break ;  }
	delete step ;

	other.print(adest) ;
	return bres ;
}

	// internal

	// construct
aart_base::aart_base()
{
}

	// destruct
aart_base:: ~ aart_base()
{
	contains.destroy() ;
}

aart_component *	aart_base::find(const char * astring, aart_component * ascope)
{
	aart_component * tmp= NULL, * step ;

	for (step= ascope; (step && (NULL == (tmp= step-> find(astring)))); step= step-> parent)
		{ }

		// try to generate
	if (NULL == tmp)
	{
		if (NULL == (tmp= aart_gen::create(astring)))
			return NULL ;
		ascope-> add(tmp) ;
	}

	tmp-> parent= ascope ;
	return tmp;
}
