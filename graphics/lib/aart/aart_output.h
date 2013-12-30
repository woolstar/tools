#ifndef	AARTOUTPUT_DEF
#	define	AARTOUTPUT_DEF	1

#include	"generaloutput.h"

class	aart_output : public generalfileoutput
{
	public:
		aart_output(const char *) ;

		bool	print(const char *) ;
		bool	printf(const char *, ...) ;

	protected:
		int		indent ;
		char  * mtab ;

		void	shift_in(void) ;
		void	shift_out(void) ;
} ;

#endif	// AARTOUTPUT_DEF