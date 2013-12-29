#ifndef	GENERAL_EXCEPTION
#	define	GENERAL_EXCEPTION	1

#include	"general.h"

#ifndef	TRAP
#	define TRAP(eee)	GeneralException::trap(eee)
#endif

#ifdef	_DEBUG

#ifndef	ASSERT
#	define	ASSERT(eee)	if (!(eee)) { TRAP("Assert"); }
#endif
#ifndef	ASSERTMESSAGE
#	define	ASSERTMESSAGE(eee, sss)	if (!(eee)) { TRAP(sss); }
#endif
#ifndef ASSERTPOINTER
#	define	ASSERTPOINTER(eee)	if (NULL == eee) { TRAP("AssertPointer"); }
#endif
#ifndef ASSERTPOINTERMESSAGE
#	define ASSERTPOINTERMESSAGE(eee, sss) if (NULL == eee) { TRAP(sss); }
#endif
#ifndef ASSERTSTRING
#	define	ASSERTSTRING(eee)	if ((NULL == eee) || ! * eee) { TRAP("AssertString"); }
#endif

#else

	// no effect

#ifndef	ASSERT
#	define	ASSERT(eee)
#endif
#	define	ASSERTMESSAGE(eee, sss)
#	define	ASSERTPOINTER(eee)
#	define	ASSERTPOINTERMESSAGE(eee, sss)
#	define	ASSERTSTRING(eee)

#endif

class	GeneralException
{
	public:
		static void	trap(const char * = NULL) ;
} ;

#endif
