	/*** epart_motion.c */
		/* parse motion headers */
extern int mofield[];
extern int ato_motion(int);

enum	art_motiont
{
	MO_CAMERA = 1,
	MO_LIGHT,
	MO_SPOT,
	MO_BEAM,
	MO_OBJECT,
	MO_INVALID
} ;

#define MOTION_COLOR	0x1
#define	MOTION_POS		0x010
#define MOTION_TAR		0x020
#define MOTION_UP		0x040
#define MOTION_SCALE	0x080
#define MOTION_Q		0x0100
#define MOTION_ROT		0x0200
#define	MOTION_FOV		0x010000

#ifdef __cplusplus
extern "C"
{
#endif

extern void		epartru_initmotion(us *, float **, float *),
				epartru_post(int, int),
				epartru_settle(int, us *, float **);
extern int		epartck_motion(void),
				epartru_motion(int, unsigned int *, int);

#ifdef	TF_SCENE_LINK
extern double	epart_motionvalue(int track, tlist_element *, int pos, ts_track *);
extern void		epart_motionvec(svector *, int track, tlist_element *, int pos, ts_track *);
extern void		epart_motionarray(float *, int nitems, int track, tlist_element *, int pos, ts_track *);

extern int	epart_setmotionvalue(float value, int offset, tlist_element * ptr, int pos, ts_track *ref);
extern int	epart_setmotionvec(svector *, int offset, tlist_element * ptr, int pos, ts_track *ref);
extern int	epart_setmotionarray(float *, int nitems, int offset, tlist_element * ptr, int pos, ts_track *ref);
#endif

#ifdef	TF_SCENE_STRUCTURES
	/*** epart_surface.c */
		/* parse surface declaration */
extern void	epartsu_installmap(tsc_map *, tsc_texture *);
extern int	epartck_material(void),
			epartru_material(tscene_surface *);
#endif

	/*** epart_rule.c */
		/* parse structure of file, allocate and load */
extern int	epart_checkparam(int),
			epart_checkval(int);

	/*** epart_util.h */
extern int	tart_seqname(char *dest, int *offset, char *word);
extern void	tart_undo_seqname(char *dest, int offset, char *word);
extern void	tart_seqform(char *dest, char *tmpl, int code, int, int);
extern void tart_seqdirect(char * dest, char * src, int );

#ifdef	TF_SCENE_STRUCTURES

	/*** epart_rule.h */
extern int	test_art(char *, tscene *);
extern int	parse_art(char *, tscene *);
extern void	release_art(tscene *);

	/*** epart_mchar.c */
extern int	tart_metatest(tscene *);
extern int	tart_selectchar(tscene * dest, tscene * src, char * cname);

#endif
#ifdef __cplusplus
} ;
#endif

