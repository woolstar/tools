#include "aar.h"

#include	"aart_core.h"
#include	"aart_node.h"

aart::aart()
{
	add(& setup_data) ;
}

aart:: ~ aart()
{
	contains.remove(& setup_data) ;
}

	// parse
aart *	aart::parse(const char * afile)
{
	aart * base= NULL ;
	FILE * fsrc ;

	fsrc= fopen(afile, "r") ;
	if (NULL == fsrc)
		return NULL ;

	base= new aart() ;

	base-> load(fsrc) ;
	base-> post() ;

	return base ;
}

void	aart::baseline(void)
{
	aart_node	* basenode ;
	aart_element	* defelem ;

	basenode= nodelist ;

	if (NULL == basenode-> cameraref)
	{
		defelem= new aart::camera("camera", basenode-> cameraref= new aart_node::cameraset(basenode)) ;
		basenode-> cameraref-> add(defelem) ;
		basenode-> add(basenode-> cameraref) ;
	}

	if (NULL == basenode-> lightref)
	{
		basenode-> lightref= new aart_node::lightset(basenode) ;
		basenode-> add(basenode-> lightref) ;
	}
}

void	aart::post(void)
{
	// create defaults for missing stuff.
}

	// find
	//	 locate known components by name
aart_component *	aart::find(const char * alabel, aart_component * abase)
{
	if (! strncmp(alabel, "setup", 5))
		return & setup_data ;

	return aart_scene::find(alabel, abase) ;
}
