#ifndef	AARTSCENE_DEF
#	define	AARTSCENE_DEF	1

#include <general.h>
#include "aart_base.h"

class	aart_node ;

class	aart_scene : public aart_base
{
	public:
		aart_scene() ;

		aart_node *
			getbase() ;
		aart_node *
			getcur();
		aart_node *
			get(const char *) ;

	protected:
		aart_node	* nodelist, * nodeend ;

		virtual aart_component *	find(const char *, aart_component *) ;

	private:
		aart_node	* curnode ;
} ;

#endif	// AARTSCENE_DEF