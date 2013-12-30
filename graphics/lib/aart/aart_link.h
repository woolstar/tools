#ifndef	AARTLINK_DEF
#	define	AARTLINK_DEF	1

#include <generalobject.h>

class	aart_element ;

class	aart_link : public generalobject
{
	public:
		aart_link(aart_element *) ;
		virtual ~ aart_link() ;

		void	* dataptr ;

		void	detach(void) { if (mjoin) mjoin-> mlink= NULL; }

	protected:
		friend	aart_element ;

		aart_element	* mjoin ;
} ;

#endif	// AARTLINK_DEF