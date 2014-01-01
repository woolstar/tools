#ifndef	DEF_EFFECTOPERATION
#	define	DEF_EFFECTOPERATION	1

#	include	<generalbag.h>

class	effect_op : public generalobject
{
	public:
		virtual ~ effect_op() { }

		virtual void	set(const char *, long) {}
		virtual void	set(const char *, float) {}
		virtual void	set(const char *, char *) {}

		static void		process(void) ;
		static void		size() ;
		static void		clean(void) { geffect.destroy(); }

	protected:
		effect_op() ;

		virtual void	op(void) = 0 ;
		virtual void	setsize(unsigned int, unsigned int) { }

		static generalbag	geffect ;
} ;

#endif	// DEF_EFFECTOPERATION