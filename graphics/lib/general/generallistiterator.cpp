// ===========================================================================
// Simple list stepper.
// ===========================================================================

#include <general.h>

#include "generallist.h"
#include "generallinkable.h"

generallistiterator::generallistiterator(generallist * alist, Direction adir)
{
	ASSERTPOINTER(alist) ;

	m_list= alist ;
	mdir= adir ;
	m_element= (kForward == adir) ? alist-> first() : alist-> last() ;

	m_list-> attach(this) ;
}

generallistiterator:: ~ generallistiterator()
{
	if (m_list)
		m_list-> detach(this) ;
}

	////

void	generallistiterator::reset()
{
	if (m_list)
		m_element= (kForward == mdir) ? m_list-> first() : m_list-> last() ;
}

bool	generallistiterator::next(generallinkable * & aref)
{
	if (! m_element)
		return false ;

	aref= m_element ;
	m_element= m_element-> m_next ;
	return true ;
}

bool	generallistiterator::previous(generallinkable * & aref)
{
	if (! m_element)
		return false ;

	aref= m_element ;
	m_element= m_element-> m_previous ;
	return true ;
}

	////

	// ignore inserts
void	generallistiterator::insert_notify(generallinkable *) { }

	// step over deletes
void	generallistiterator::remove_notify(generallinkable * arec)
{
	if (m_element != arec) return ;

	switch (mdir)
	{
		case kForward:  m_element= m_element-> m_next ;  break ;
		case kBackward:  m_element= m_element-> m_previous ;  break ;
	}
}

	// shutdown if this is our list
void	generallistiterator::delete_notify(generallist * alist)
{
	ASSERT(alist == m_list) ;

	m_list= NULL ;
	m_element= NULL ;
}