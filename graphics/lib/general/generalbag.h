
#ifndef	GENERALBAG_DEF
#	define	GENERALBAG_DEF	1

#include	"generallist.h"
#include	"generallinkable.h"

class	generalbagstepper ;

class	generalbag : public generallist
{
	public:
		~ generalbag() ;

		void	insert(generalobject *) ;
		void	append(generalobject *) ;
		void	remove(generalobject *) ;

		bool	contains(generalobject *) ;

		void	destroy(void) ;

	protected:
		friend generalbagstepper ;

		class node : public generallinkable
		{
			public:
				node(generalobject * aptr) { m_contents= aptr; }
				~ node() { }

				generalobject	* contents(void) { return m_contents; }
				void	destroy(void) ;
			protected:
				generalobject	* m_contents ;
		} ;

		node	* findcontainer(generalobject *) ;
} ;

class	generalbagstepper	: public generallistiterator
{
	public:
		generalbagstepper(generalbag * abag, Direction adir) : generallistiterator(abag, adir) { }
		~ generalbagstepper() { }

		bool	next(generalobject	* &) ;
		bool	previous(generalobject * &) ;
} ;

template <class T> class tbagstepper : public generalbagstepper
{
	public:
		tbagstepper(generalbag * abag, Direction adir = kForward) : generalbagstepper(abag, adir) { }

		bool	next(T & aref)
		{
			generalobject	* elemptr ;
			bool	bret= generalbagstepper::next(elemptr) ;
			aref= (T) static_cast<T>(elemptr) ;
			return bret ;
		}
		bool	previous(T & aref)
		{
			generalobject	* elemptr ;
			bool	bret= generalbagstepper::previous(elemptr) ;
			aref= (T) static_cast<T>(elemptr) ;
			return bret ;
		}
} ;


#endif	// GENERALBAG_DEF