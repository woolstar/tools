#include "aar.h"

#include "aart_node.h"

aart_node::aart_node(aart_scene * ascene, const char * astring)
{
	mtype= eUnknown ;

	if (astring && * astring)
	{
		generalstring::captureKnown(mname, astring) ;
		mtype= eCharacter ;
	}
	else
	{
		* mname= '\0' ;
		mtype= eScene ;
	}

	cameraref= NULL ,
	lightref= NULL ,
	surfaceref= NULL ,
	geometryref= NULL ;

	container= ascene ;
	next= NULL ;
}

	// parse
	//	no local commands
bool	aart_node::parse(const char * astring, FILE * fsrc)
{
	return aart_component::parse(astring, fsrc) ;
}

bool	aart_node::print(aart_output * adest)
{
	switch (mtype)
	{
		case eUnknown :
			return false ;

		case eScene:
			adest-> print("{scene\n") ;
			break ;
		case eCharacter:
			adest-> printf("{art'%s\n", mname) ;
			break ;
	}

	return aart_component::print(adest) ;
}

aart_component *	aart_node::find(const char * alabel)
{
	aart_component * ptr = NULL ;

	if (! strncmp(alabel, "cameras", 7))
	{
		if (NULL == lightref)
		{
			cameraref= new cameraset(this) ;
			add(cameraref) ;
		}
		ptr= cameraref ;
	}
	else
	if (! strncmp(alabel, "lights", 6))
	{
		if (NULL == lightref)
		{
			lightref= new lightset(this) ;
			add(lightref) ;
		}
		ptr= lightref ;
	}
	else
	if (! strncmp(alabel, "geometry", 8))
	{
		if (NULL == geometryref)
		{
			geometryref= new geometryset(this) ;
			add(geometryref) ;
		}
		ptr= geometryref ;
	}
	else
	if (! strncmp(alabel, "surfaces", 8))
	{
		if (NULL == surfaceref)
		{
			surfaceref= new surfaceset(this) ;
			add(surfaceref) ;
		}
		ptr= surfaceref ;
	}

	return ptr ;
}

	//// sets

	// camera
bool	aart_node::cameraset::print(aart_output * adest)
{
	adest-> print("{cameras\n") ;
	return aart_component::print(adest) ;
}

aart_component *	aart_node::cameraset::find(const char * alabel)
{
	aart_component * ptr ;

	if (! strncmp(alabel, "camera", 5))
	{
		char * str ;

		str= strchr(const_cast<char *>(alabel), '\'') ;
		if (str)
		{
			str ++ ;
			ptr= new aart::camera(str, this) ;
			add(ptr) ;
			return ptr ;
		}
	}

	return NULL ;
}

aart::camera *	aart_node::cameraset::retrieve(const char * aname)
{
	return NULL ;
}

	// light

bool	aart_node::lightset::parse(const char * astring, FILE *)
{
	if (! strncmp(astring, "ambient", 7))
	{
		if (! aart_element::parsecolor(ambient, astring))
			return false ;
	}
	else
		ignore(astring) ;

	return true ;
}

bool	aart_node::lightset::print(aart_output * adest)
{
	adest-> print("{lights\n") ;
	adest-> printf("ambient %g %g %g\n", C_expandl(ambient) ) ;

	return aart_component::print(adest) ;
}

aart_component *	aart_node::lightset::find(const char * alabel)
{
	aart_component * ptr ;

	if (! strncmp(alabel, "light", 5))
	{
		char * str ;

		str= strchr(const_cast<char *>(alabel), '\'') ;
		if (str)
		{
			str ++ ;
			ptr= new aart::light(str, this) ;
			add(ptr) ;
			return ptr ;
		}
	}

	return NULL ;
}

	// geom
bool	aart_node::geometryset::print(aart_output * adest)
{
	adest-> print("{geometry\n") ;
	return aart_component::print(adest) ;
}

aart_component *	aart_node::geometryset::find(const char * alabel)
{
	aart_component * ptr ;

	if (! strncmp(alabel, "actor", 5))
	{
		char * str ;

		str= strchr(const_cast<char *>(alabel), '\'') ;
		if (str)
		{
			str ++ ;
			ptr= new aart::geometry(str, this) ;
			add(ptr) ;
			return ptr ;
		}
	}

	return NULL ;
}

	// surface
bool	aart_node::surfaceset::print(aart_output * adest)
{
	adest-> print("{surfaces\n") ;
	return aart_component::print(adest) ;
}

aart_component *	aart_node::surfaceset::find(const char * alabel)
{
	aart_component * ptr ;

	if (! strncmp(alabel, "surface", 7))
	{
		char * str ;

		str= strchr(const_cast<char *>(alabel), '\'') ;
		if (str)
		{
			str ++ ;
			ptr= new aart::surface(str, this) ;
			add(ptr) ;
			return ptr ;
		}
	}

	return NULL ;
}

aart::surface *	aart_node::surfaceset::retrieve(const char * aname)
{
	tbagstepper<aart::surface *> step(& contains) ;
	aart::surface * surfptr ;

	while (step.next(surfptr)) { if (! strcmp(surfptr-> getname(), aname)) return surfptr ; }
	return NULL ;
}

