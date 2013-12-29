// ===========================================================================
// Simple collection.
// ===========================================================================

#include <general.h>
#include "generalbag.h"

	// destructor - cleanup locally owned nodes.
generalbag:: ~ generalbag()
{
	tlistiterator<node *>	step(this) ;
	node * element ;

	while (step.next(element)) { delete (element) ; }
}

	// make a wraper and add to list
void	generalbag::insert(generalobject * aptr)
{
	generallist::insert(new node(aptr)) ;
}
void	generalbag::append(generalobject * aptr)
{
	generallist::append(new node(aptr)) ;
}

	// find the container, remove from internal list & dispose of container.
void	generalbag::remove(generalobject * aptr)
{
	node * nodeptr= findcontainer(aptr) ;

	if (NULL != nodeptr)
	{
		generallist::remove(nodeptr) ;
		delete nodeptr ;
	}
}

	// destroy
	//	remove all contents, deleting contained objects as well
void	generalbag::destroy()
{
	node	* element= (node *) m_first ;

	m_first= m_last= NULL ;
	while (element)
	{
		node * nextp= (node *) element-> next() ;

		element-> destroy() ;
		delete (element) ;

		element= nextp ;
	}

	m_count= 0;
}

	// look through list for a container that holds aptr.
bool	generalbag::contains(generalobject * aptr)
{
	for (node * step= static_cast<node *>(m_first); (step); step= (node *) (step-> next()))
		{ if (aptr == step-> contents()) return true ; }

	return false ;
}

	// step through list and return container that holds aptr if found
generalbag::node	* generalbag::findcontainer(generalobject * aptr)
{
	for (node * step= static_cast<node *>(m_first); (step); step= (node *) (step-> next()))
		{ if (aptr == step-> contents()) return step ; }

	return NULL ;
}

void	generalbag::node::destroy()
{
	delete	m_contents ;
	m_list= NULL, m_next= NULL, m_previous= NULL ;
}


	//// generalbag iterator

bool	generalbagstepper::next(generalobject	* & aref)
{
	void	* node= NULL ;
	bool	resu= generallistiterator::next((generallinkable *&) node) ;

	if (NULL != node)
		aref= (generalobject *) (((generalbag::node *) node) -> contents()) ;

	return resu ;
}
bool	generalbagstepper::previous(generalobject * &aref)
{
	void	* node= NULL ;
	bool	resu= generallistiterator::previous((generallinkable *&) node) ;

	if (NULL != node)
		aref= (generalobject *) (((generalbag::node *) node) -> contents()) ;

	return resu ;
}

