#ifndef	AARTCORE_DEF
#	define	AARTCORE_DEF	1

#include	"aart_scene.h"
#include	"aart_element.h"

class	aart : public aart_scene
{
	public:
		aart() ;
		~ aart() ;

		void	baseline(void) ;

		static aart * parse(const char *) ;

	public:
		class	camera : public aart_element
		{
			public:
				camera(const char *, aart_component *) ;

				vector
					position ;
				quaternion
					direction ;
				float
					fov,
					focus ;

				enum ImportSource
				{
					eNone = 0,
					eMotion,
					eFile,
				};

				ImportSource	mimport;
				char	importsrc[128] ;

			protected:
				bool	parse(const char *, FILE *);
				bool	print(aart_output *);
		} ;

		class	light : public aart_element
		{
			public:
				light(const char *, aart_component *) ;

				enum LightType
				{
					eUnknown = 0,
					eBeam ,
					ePoint ,
					eSpot ,
				} ;

				LightType	mtype ;
				float	bright ;
				color	mcolor ;
				vector
					position,
					direction ;

				float	spot ;
				bool	mshadow ;

			protected:
				bool	parse(const char *, FILE *);
				bool	print(aart_output *);
		} ;
		class	geometry : public aart_element
		{
			public:
				geometry(const char *, aart_component *) ;

				vector
					position,
					scale ;

				bool	doserv ;
				generalstringlist	src ;

				char	config[256], motion[256] ;

				bool	parse(const char *, FILE *);
				bool	print(aart_output *);
		} ;
		class	texture ;
		class	surface : public aart_element
		{
			public:
				surface(const char *, aart_component *) ;
				~ surface() ;

				enum SurfaceAttributes
				{
					eNone = 0,
					eAmbient = 0x01,
					eGlow = 0x02,
					eDiffuse = 0x04,
					eSpecular = 0x08,
					eReflect = 0x10,
					eGlass = 0x20,
				} ;

				enum SurfaceMode
				{
					eStandard = 0,
					eAlphaLite,
					eLiquid,
					ePosterize,
				} ;

				unsigned long
					muse,
					clink,
					layer ;
				SurfaceMode	smode ;
				color
					ambient,
					glow,
					diffuse,
					specular,
					reflect,
					glass ;
				float
					gloss,
					refraction,
					alpha,
					za, zb, zc ;

				bool	isinvis;	// isalphalite ;

				bool	parse(const char *, FILE *);
				bool	print(aart_output *);

				bool	hastexture(void) { return (contains.getcount() > 0) ; }
				int		texturecount(void) { return contains.getcount(); }
				tbagstepper<texture *>	* getstep(void) ;

			protected:
				int		parsez(const char *) ;
				bool	test_texture(const char *) ;
				void	leavescope(void) ;
		} ;
		class	texture : public aart_component
		{
			public:
				enum	Type
				{
					eNone = 0,
					eColor,
					eBump,
					eEnvir,
				} ;

				texture(Type, const char *, aart_component *) ;

				static Type	atotype(const char *) ;

				Type	mtype ;
				char	mapname[256] ;

				bool	parse(const char *, FILE *) { return false; }
				bool	print(aart_output *);
		} ;

		class	setup : public aart_component
		{
			public:
				setup() ;

				int	x, y ;
				float	screen_aspect, pixel_aspect ;

				enum	BackgroundType
				{
					eNone= 0,
					eColor= 1,
					eImage= 2,
				} ;

				BackgroundType	background ;
				color	bc ;
				char	bimage[256] ;

			protected:
				bool	parse(const char *, FILE *) ;
				bool	print(aart_output *) ;

				void	leavescope(void) ;

				int	aspect_x, aspect_y ;
		} ;

		setup	setup_data ;

	protected:
		void	post() ;

		virtual aart_component *	find(const char *, aart_component *) ;
} ;

#endif	// AARTSURFACE_DEF
