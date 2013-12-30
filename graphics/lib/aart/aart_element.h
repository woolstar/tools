#ifndef AARTELEMENT_DEF
#	define	AARTELEMENT_DEF	1

#include	"aart_base.h"

#include	"bcolor.h"
#include	"bq.h"
#include	"bvek.h"

class	aart_link ;

class	aart_element : public aart_component
{
	public:
		aart_element(const char *, aart_component *) ;
		~ aart_element() ;

		static bool	parsevec(vector &, const char *) ;
		static bool	parseq(quaternion &, const char *) ;
		static bool	parsecolor(color &, const char *) ;
		static bool	parseval(float &, const char * ) ;
		static bool	parsepoint(int &, int & , const char *) ;

		const char *	getname(void) const { return mname; }

		aart_link *		mlink ;

	protected:
		char	mname[128] ;
} ;

#endif	// AARTELEMENT_DEF