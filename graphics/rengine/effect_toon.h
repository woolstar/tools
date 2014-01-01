#ifndef	DEF_TOONEFFECT
#	define	DEF_TOONEFFECT	1

#if 0
#	include "effect_op.h"
#	include "JtToonProcessor.h"

class	toon_effect : public effect_op
{
	public:
		toon_effect(unsigned int awidth, unsigned int aheight, float aratio) ;
		~ toon_effect() ;

			// effect
		void	set(const char *, float) ;
		void	set(const char *, char *) ;

		void	op(void) ;

		void	setsize(unsigned int, unsigned int) ;

	private:
		JtToonProcessor	* proc ;

		float	cres, tres ;
} ;

#endif

#endif	// DEF_TOONEFFECT
