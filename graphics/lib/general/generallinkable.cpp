
#include <general.h>

#include "generallinkable.h"
#include "generallist.h"

generallinkable::generallinkable()
{
	m_list= NULL ;
	m_next= m_previous= NULL ;
}

generallinkable:: ~ generallinkable()
{
	if (m_list) m_list-> remove(this) ;
}