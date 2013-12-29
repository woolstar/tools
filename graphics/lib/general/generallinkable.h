#ifndef	GENERALLINKABLE_DEF
#	define	GENERALLINKABLE_DEF	1

#include "generalobject.h"

class	generallist ;
class	generallistiterator ;

class	generallinkable : public generalobject
{
	public:
		generallinkable() ;
		~ generallinkable() ;

		friend	generallist ;
		friend	generallistiterator ;

		virtual generallinkable	* next(void) { return m_next; }
		virtual generallinkable	* previous(void) { return m_previous; }

	protected:
		generallist	* m_list ;
		generallinkable	* m_next, * m_previous ;
} ;

#endif	// GENERALLINKABLE_DEF