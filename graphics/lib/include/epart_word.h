
enum art_token
{
	_INF = 1,
	_SIZE,
	_COLOR,
	_AMBIENT,
	_SCALE,
	_SOLID,
	_INVIS,

	_SCENE_ACTIVE,
	_SCENE_ADAPT,
	_SCENE_ASPECT,
	_SCENE_BACKGR,
	_SCENE_BEAM,
	_SCENE_BOX,
	_SCENE_BRIGHT,
	_SCENE_BUFFER,
	_SCENE_BUFTOL,
	_SCENE_CHARACTER,
	_SCENE_COMPLEX,
	_SCENE_CONE,
	_SCENE_DEPTH,
	_SCENE_EXEC,
	_SCENE_FALLOFF,
	_SCENE_FILTER,
	_SCENE_FLAT,
	_SCENE_FLIP,
	_SCENE_FOG,
	_SCENE_FOGCOLOR,
	_SCENE_FORMAT,
	_SCENE_GAMMA,
	_SCENE_GAUSS,
	_SCENE_HORIZON,
	_SCENE_IMAGE,
	_SCENE_INFSPH,
	_SCENE_LENS,
	_SCENE_LIGHT,
	_SCENE_LINEAR,
	_SCENE_LOWER,
	_SCENE_MATERIAL,
	_SCENE_MORPH,
	_SCENE_NEARFAR,
	_SCENE_NOISY,
	_SCENE_OB,
	_SCENE_OBJECT,
	_SCENE_PATH,
	_SCENE_ROOT,
	_SCENE_SERV,
	_SCENE_PARENT,
	_SCENE_PICTURE,
	_SCENE_POST,
	_SCENE_POWER,
	_PREPARE,
	_SCENE_REAL,
	_SCENE_REFLECTED,
	_SAFE,
	_SCENE_SETUP,
	_SCENE_SHADOW,
	_SCENE_SOFT,
	_SCENE_SPHEREL,
	_SCENE_SPOT,
	_SCENE_LANCZOS,
	_SCENE_TRACKS,
	_SCENE_TRI,
	_SCENE_UPPER,
	_SCENE_USE,
	_SCENE_VIEW,
	_SCENE_WARPVIEW,
	_SCENE_WIDEVIEW,
	_SCENE_WINDOW,
	_SCENE_IMPORT,
	_SCENE_WORLD,
	_SCENE_ZONE,

	_MOTION,
	_MOTION_FOV,
	_MOTION_POSITION,
	_MOTION_Q,
	_MOTION_ROTATION,
	_MOTION_TARGET,
	_MOTION_UP,
	_MOTION_META,
	_MOTION_META3,

	_SURFACE,
	_END,
	_SURF_ALL,
	_SURF_ALPHA,
	_SURF_ALPHALITE,
	_SURF_ALT,
	_SURF_BUMP,
	_SURF_BUMPY,
	_SURF_DIFFUSE,
	_SURF_EXPO,
	_SURF_GLASS,
	_SURF_GLASSBACK,
	_SURF_GLOSS,
	_SURF_KA,
	_SURF_KD,
	_SURF_KL,
	_SURF_KR,
	_SURF_KS,
	_SURF_KT,
	_SURF_INDEX,
	_SURF_LIQUID,
	_SURF_LUMINANT,
	_SURF_MIXGLOSS,
	_SURF_NONE,
	_SURF_REFLECT,
	_SURF_ROUGH,
	_SURF_SPECULAR,
	_SURF_SHADES,
	_SURF_TRANSMIT,
	_SURF_TEXTURE,
	_SURF_MIX,
	_SURF_RENVIR,
	_SURF_WRAP,

	_SURF_MAP,
	_SURF_PROJECT,
	_SURF_CUBIC,
	_SURF_CYLNDER,
	_SURF_MIRROR,
	_SURF_NORMAL,
	_SURF_PLANAR,
	_SURF_SPHERE,
	_SURF_SQUARECYL,

	_SURF_DARKC,
	_SURF_EDGERANGE,
	_SURF_GRAIN,
	_SURF_MAINRANGE,
	_SURF_MARBLESV,
	_SURF_SWIRL,
	_SURF_SWIRLFREQ,
	_SURF_STONESV,
	_SURF_SEEDCOLOR,
	_SURF_TURB,
	_SURF_VEINRANGE,
	_SURF_WOODSV,

		/* junk */
	_SCENE_GRID,
	_SCENE_HUV,
	_SCENE_MESSAGES,
	_SCENE_NAME,
	_SCENE_NAMES,
	_SCENE_NAMEFORM,
	_SCENE_REVERSE,
	_SCENE_RGB,
	_SURF_TGA,
	_SURF_VID
} ;

#define	SCAN	i= scan_art(&av)
#define hcUSE	epart_holdc= 0;
#define	hcSCAN	epart_holdc= 0, i= scan_art(&av)

#define	AHOLDI	atoi(epart_hold)
#define AHOLDF	atof(epart_hold)

#define isINT	(1 == i)
#define isNUM	((1 == i) || (2 == i))
#define isWORD	((3 <= i) && (i <= 5))
#define isTOK(xx)	((3 == i) && (xx == av))

	/*** epart_word.c */
		/* breakup file into words */
extern int	epart_scannerline(char *, int),
			scan_art(long *);
extern int	open_art(char *);
extern void	rewind_art(void),
			close_art(void);
