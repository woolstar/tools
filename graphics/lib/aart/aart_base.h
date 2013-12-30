#ifndef	AARTBASE_DEF
#	define	AARTBASE_DEF	1

#include <generalobject.h>
#include <generalbag.h>
#include <generalstringlist.h>

class	aart_base ;

#include	"aart_output.h"

class	aart_component : public generalobject
{
	public:
		aart_component() ;
		aart_component(aart_component *) ;
		virtual ~ aart_component() ;

		virtual bool	parse(const char *, FILE *) = 0;
		virtual bool	print(aart_output *) = 0;

		void	add(aart_component *) ;
		void	ignore(const char *) ;

	protected:
		friend aart_base ;

		aart_component	* parent ;

			// handle context tracking
		virtual void	leavescope(void) {}

		virtual aart_component *	find(const char *) { return NULL ; }
		void	remove(aart_component *) ;

		generalbag	contains ;
		generalstringlist	other ;

		static unsigned long	atocode(const char *) ;
} ;

class	aart_base
{
	public:
		aart_base() ;
		virtual ~ aart_base() ;

		static aart_base *	parse(const char *) ;

		void	add(aart_component *) ;
		void	ignore(const char *) ;

		bool	print(const char *) ;

	protected:
		generalbag	contains ;
		generalstringlist	other ;

		void	load(FILE *) ;
		bool	print(aart_output *) ;

		virtual aart_component *	find(const char *, aart_component *) ;
} ;

#endif	// AARTBASE_DEF