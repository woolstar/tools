
#ifndef TF_SCENE_LINK
#	define	TF_SCENE_LINK

#ifndef TF_SCENE_STRUCTURES
#	include "tscene.h"
#endif

thing  TLIST_element
{
	char name[64];

	unsigned long type, attr;
	unsigned int
		motion;
	float * tracks;

	union
	{
		te_light light;
		te_object object;
	} dat;

	te_system *cmd;
	struct TLIST_element
		* parent, * next;
} tlist_element;

thing
{
	int
		ctlight,
		ctobject;
	tlist_element
		* lights,
		* objects;
} tl_element;

thing
{
	int count;
	tlist_element ** list;
} tl_contain;

thing
{
	ts_cameras cam;
	ts_setup setup;
	ts_character chrs;
	tl_element elem;
	ts_material mat;
	tl_contain set;
	ts_track tracks;
	ts_mem memory;
} tlist_scene;

#	ifdef __cplusplus
extern "C" {
#	endif

	/*** utilties */

extern int	createElementImmediate(tlist_element ** dest, long type) ;
extern int	insertElement(tlist_element * item, tlist_scene * ref, int place) ;  /* 0 = at the end */
extern int	motionImmediate(tlist_element * dest, tlist_scene * );

extern void	removeElement(tlist_element * item, tlist_scene * ref) ;
extern void	freeElement(tlist_element * item) ;

extern int	parse_artlist(char * name, tlist_scene * ptr) ;
extern void	release_artlist(tlist_scene * ptr) ;

	/** tscenesave.c */
extern int	print_art(char * name, tscene * ); 
extern int	print_artlist(char * name, tlist_scene *) ;

	/*** import */
extern int	tsimport_test(ts_cameras *);
extern int	tsimport_load(ts_cameras *);
extern float	tsimport_motionvalue(int trackoff, int pos, tsrc_import *);
extern void		tsimport_motionvec(svector *, int trackoff, int pos, tsrc_import *);

#	ifdef __cplusplus
}
#	endif

#endif

