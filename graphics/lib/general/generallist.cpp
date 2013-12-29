// ===========================================================================
// Simple list.
// ===========================================================================

#include <general.h>

#include "generallist.h"
#include "generallinkable.h"

generallist::generallist()
{
	m_count= 0 ;

	m_first= NULL, m_last= NULL ;
	m_iterator= NULL ;
}

generallist:: ~ generallist()
{
	generallinkable	* ptr= m_first ;

	while (ptr)
	{
		generallinkable	* nextp= ptr-> m_next ;

		ptr-> m_list= NULL ;
		ptr-> m_next= ptr-> m_previous= NULL ;
		ptr= nextp ;
	}

	if (m_iterator)
	{
		m_iterator-> delete_notify(this) ;
		detach(m_iterator) ;
	}
}

void	generallist::insert(generallinkable	* aelem)
{
	if (m_iterator) m_iterator->insert_notify(aelem) ;

	aelem-> m_previous= NULL ;
	aelem-> m_next= m_first ;
	aelem-> m_list= this ;

	if (m_first) { m_first-> m_previous= aelem; }
		else { m_last= aelem; }
	m_first= aelem ;

	m_count ++ ;
}

void	generallist::append(generallinkable	* aelem)
{
	if (m_iterator) m_iterator->insert_notify(aelem) ;

	aelem-> m_previous= m_last ;
	aelem-> m_next= NULL ;
	aelem-> m_list= this ;

	if (m_last) { m_last-> m_next= aelem; }
		else { m_first= aelem; }
	m_last= aelem ;

	m_count ++ ;
}

void	generallist::remove(generallinkable * aelem) 
{
	if (m_iterator) m_iterator->remove_notify(aelem) ;

	if (NULL == aelem->m_previous)
		m_first= aelem-> m_next ;
	else
		aelem-> m_previous-> m_next= aelem-> m_next ;

	if (NULL == aelem-> m_next)
		m_last= aelem-> m_previous ;
	else
		aelem-> m_next-> m_previous= aelem-> m_previous ;

	aelem-> m_list= NULL ;
	aelem-> m_next= NULL, aelem-> m_previous= NULL ;

	m_count -- ;
}

	// scan list for element, return true if found
bool	generallist::contains(generallinkable *aelem) 
{
	for (generallinkable * step= m_first; (step); step= step-> m_next)
		{ if (aelem= step) return true ; }

	return false ;
}

	// handle a single stepper
void	generallist::attach(generallistiterator * aptr)
{
	ASSERT(NULL == m_iterator) ;
	m_iterator= aptr ;
}

void	generallist::detach(generallistiterator * aptr)
{
	ASSERT(aptr= m_iterator) ;
	m_iterator= NULL ;
}
