#include "aar.h"

#include "aart_scene.h"
#include "aart_node.h"

aart_scene::aart_scene()
{
	nodelist= new aart_node(this, "") ;

	nodeend= nodelist ;
	curnode= nodelist ;
	add(curnode) ;
}

	/// access nodes
	// getbase
aart_node *	aart_scene::getbase()
{
	return nodelist ;
}

aart_node * aart_scene::getcur()
{
	aart_node * sear = nodelist ;

	while (sear && (NULL == sear-> geometryref)) { sear= sear-> next; }
	if (! sear) sear= nodelist ;
	return sear ;
}

	// get
aart_node *	aart_scene::get(const char * aname)
{
	aart_node * sear ;

	ASSERTPOINTER(aname) ;
	if (! * aname) return NULL ;

	for (sear= nodelist; (sear && strcmp(sear-> mname, aname)); sear= sear-> next) { }
	return sear ;
}


	//// internal

	// find
	//	look for components or create character

aart_component *	aart_scene::find(const char * alabel, aart_component * abase)
{
	aart_node * ptr ;

	if (! strncmp(alabel, "shot", 4))
		return (aart_component *) 2;

		// retrieve base scene node
	if (! strncmp(alabel, "scene", 5))
		return nodelist ;

		// retrieve character node pointers (or create)
	if (! strncmp(alabel, "art", 3))
	{
		char * str ;

		str= strchr(alabel, '\'') ;
		if (str)
		{
			char tmpstr[64] ;

			str ++ ;
			generalstring::captureKnown(tmpstr, str) ;
			ptr= get(tmpstr) ;

				// create if not found
			if (! ptr)
			{
				ptr= new aart_node(this, tmpstr) ;
				add(ptr) ;

				nodeend-> next= ptr ;
				nodeend= ptr ;
			}

			curnode= ptr ;
			return ptr ;
		}
	}

	if (NULL == abase)
		abase= nodelist ;

	return aart_base::find(alabel, abase) ;
}
