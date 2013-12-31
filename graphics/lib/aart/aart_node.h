#ifndef	AARTNODE_DEF
#	define	AARTNODE_DEF	1

#include	"aart_core.h"

template <class U> class tnode_set : public aart_component
{
	public:
		tnode_set(aart_component * acomp) : aart_component(acomp)
			{}

		bool
			parse(const char * a, FILE * f)
		{ return aart_component::parse(a, f) ; }

		tbagstepper<U *> *
			getstep(void)
		{ return new tbagstepper<U *>(& contains); }
} ;

class	aart_node : public aart_component
{
	public:
		aart_node(aart_scene *, const char *) ;

		bool	parse(const char *, FILE *) ;
		bool	print(aart_output *) ;

		enum NodeType
		{
			eUnknown = 0,
			eScene,
			eCharacter,
		} ;

		NodeType gettype(void) { return mtype; }

		class	cameraset : public tnode_set<aart::camera>
		{
			public:
				cameraset(aart_component * a) : tnode_set<aart::camera>(a) { }

				bool	print(aart_output *);

				aart::camera *	retrieve(const char *) ;

			protected:
				aart_component *	find(const char *) ;
		} ;
		class	lightset : public tnode_set<aart::light>
		{
			public:
				lightset(aart_component * a) : tnode_set<aart::light>(a) { ambient.r= ambient.g= ambient.b= (float) 0.33333 ; }

				color	ambient ;

				bool
					parse(const char * a, FILE * f) ;

				bool	print(aart_output *);

			protected:
				aart_component *	find(const char *) ;
		} ;
		class	geometryset : public tnode_set<aart::geometry>
		{
			public:
				geometryset(aart_component * a) : tnode_set<aart::geometry>(a) { }

				generalstringlist	paths ;

				bool	print(aart_output *);

			protected:
				aart_component *	find(const char *) ;
		} ;
		class	surfaceset : public tnode_set<aart::surface>
		{
			public:
				surfaceset(aart_component * a) : tnode_set<aart::surface>(a) { }

				bool	print(aart_output *);

				aart::surface *	retrieve(const char *) ;

			protected:
				aart_component *	find(const char *) ;
		} ;

		cameraset	* cameraref ;
		lightset	* lightref ;
		geometryset	* geometryref ;
		surfaceset	* surfaceref ;

	protected:
		aart_component *	find(const char *) ;
		// void	leavescope(void) ;

		NodeType mtype  ;

		char	mname[128] ;
		aart_scene	* container ;

		friend class aart_scene ;

		aart_node	* next ;
} ;

#endif	// AARTNODE_DEF
