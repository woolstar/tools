#ifndef	GENERALOUTPUT_DEF
#	define	GENERALOUTPUT_DEF	1

#	include "generalobject.h"

class	generaloutput : public generalobject
{
	public:
		generaloutput() {}
		virtual ~ generaloutput() {}

		virtual bool	print(const char *) = 0;
		virtual bool	printf(const char *, ...) = 0;
} ;

class	generalfileoutput : public generaloutput
{
	public:
		generalfileoutput() ;
		generalfileoutput(const char *) ;
		~ generalfileoutput() ;

		virtual bool	isvalid(void) { return (NULL != fdest); }

		bool	print(const char *) ;
		bool	printf(const char *, ...) ;

	protected:
		FILE	* fdest ;
} ;

#endif	// GENERALOUTPUT_DEF