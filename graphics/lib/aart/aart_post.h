#ifndef	AARTPOST_DEF
#	define	AARTPOST_DEF

#include "aart_base.h"
#include "aart_core.h"


class aart_post : public aart_component
{
	public:
		aart_post(const char *) ;
		~ aart_post() ;

	public:
		unsigned int
			adapt ;
		unsigned int
			rdepth, tdepth ;

		aart::camera * getactive(tbagstepper<aart::camera *> *) const ;

		enum FilterType
		{
			eNone= 0,
			eBox,
			eGauss,
			eSync,
		} ;
		FilterType	filter ;
		unsigned int
			filterwindow ;

		char	destpath[256] ;

		bool	textureflip ;

	public:
		class	effect : public aart_element
		{
			public:
				effect(const char * aname, aart_component * apar) : aart_element(aname, apar) {}

				virtual const char * gettype(void) = 0 ;
		} ;

		tbagstepper<effect *> * getstep(void) const { return new tbagstepper<effect *>((generalbag *) & contains) ; }

		class	toon : public effect
		{
			public:
				toon(const char *, aart_component *) ;

					// aart_effect
				const char * gettype(void) { return "toon" ; }

				float
					cline_w, cline_h, cline_t,
					tline_w, tline_h, tline_t ;

				color	line[16] ;
				unsigned short	luse ;

			protected:
				bool	parse(const char *, FILE *);
				bool	print(aart_output *);
		} ;

	public:
		static const aart_post *	getpost(void) ;
		static const aart_post *	getpost(aart_node *) ;

	protected:
			// component
		bool	parse(const char *, FILE *) ;
		bool	print(aart_output *) ;

		aart_component *	find(const char *) ;

		char	mcamera[64] ;

		static aart_post	* base ;
} ;

#endif
