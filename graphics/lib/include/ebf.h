typedef struct
{
	char name[32];
	float r, g, b;
} bf_surface;

typedef struct
{
	int
		vcount, ncount, ucount,
		scount, fcount, timesize;
	char
		*vptr, *nptr, *uptr,
		*sptr, *fptr, *tptr;
} bf_container;

typedef struct
{
	float x, y;
} bf_flat;

#ifdef __cplusplus
extern "C"
{
#endif

extern int		load_embf(FILE *, bf_container *);
extern int		mem_epbf(long, char *, bf_container *);
extern int		store_embf(FILE *, bf_container *);
extern void		clear_embf(bf_container *ptr) ;

extern int		epbf_sizeface(unsigned short *, int, vector *);
extern int		epbf_createface(unsigned short *, long *, unsigned short *, int, int, vector *);

extern int		parse_obj(const char *name, bf_container *ptr) ;

#ifdef	TF_RENDER_STRUCTURES

int tobj_load(FILE *, tro_polys *);
int tobj_mem(long sz, char * data, tro_polys *);
int	tobj_conv(bf_container *, tro_polys *) ;
int tobj_save(FILE *, tro_polys *);
int tobj_loadname(char *, tro_polys *);
int tobj_savename(char *, tro_polys *);
#endif

#ifdef __cplusplus
} ;
#endif

#ifndef EPB_FACE
#	define	EPB_FACE	0x4000
#	define EPB_FACTN	0x1000
#	define EPB_FACTT	0x2000
#	define EPB_FACTM	0x3000
#	define EPB_ATTRIB	0x8000
#	define EPB_SURF	0x8400
#	define EPB_BASIS	0x8800
#	define EPB_EEEK	0x8c00
#	define EPBA_UNFRACT	0x0100

#	define	ES_VERSION	0x04010000
#endif

#ifdef WIN32
#	pragma warning( disable : 4018)
#	pragma warning( disable : 4244)
#	pragma warning( disable : 4305)
#endif

