#ifndef	GENERALLIST_DEF
#	define	GENERALLIST_DEF	1


#include	"generalobject.h"

class	generallinkable ;
class	generallistiterator ;

	// basic list
class	generallist : public generalobject
{
	public:
		generallist() ;
		~ generallist() ;

		friend generallistiterator ;

		unsigned long	getcount(void)  { return m_count; }

		void	insert(generallinkable *) ;
		void	append(generallinkable *) ;
		void	remove(generallinkable *) ;

		bool	contains(generallinkable *) ;
		bool	isempty(void) { return NULL == m_first; }

		generallinkable	* first(void) { return m_first; }
		generallinkable	* last(void) { return m_last; }

	protected:
		void	attach(generallistiterator *) ;
		void	detach(generallistiterator *) ;

		unsigned long	m_count ;

		generallinkable	* m_first, * m_last ;
		generallistiterator	* m_iterator ;
} ;

	// iterator
class	generallistiterator
{
	public:

		typedef enum { kForward= 1, kBackward= -1 } Direction ;

		generallistiterator(generallist *, Direction = kForward) ;
		~ generallistiterator() ;

		friend	generallist ;

		void	reset(void) ;
		bool	next(generallinkable *&) ;
		bool	previous(generallinkable *&) ;

	protected:
		void	insert_notify(generallinkable *) ;
		void	remove_notify(generallinkable *) ;
		void	delete_notify(generallist *) ;

		generallist *	m_list ;
		Direction	mdir ;

		generallinkable	* m_element ;
} ;

template <class T> class tlistiterator : public generallistiterator
{
	public:
		tlistiterator(generallist * alist, Direction adir = kForward) : generallistiterator(alist, adir) { }
		~ tlistiterator() { } ;

		bool	next(T & aref)
		{
			generallinkable	* elemptr ;
			bool	bret= generallistiterator::next(elemptr) ;
			aref= (T) static_cast<T>(elemptr) ;
			return bret ;
		}
		bool	previous(T & aref)
		{
			generallinkable	* elemptr ;
			bool	bret= generallistiterator::previous(elemptr) ;
			aref= (T) static_cast<T>(elemptr) ;
			return bret ;
		}
} ;


#endif	//	GENERALLIST_DEF