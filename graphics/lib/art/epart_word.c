#include "basic.h"
/** #include "eart.h" */
#include "epart_word.h"

   /*****/

static	FILE	*fu;
static int	epart_holdn;
		int		epart_holdc;
		char	epart_hold[SLINE];

#define	FNUMBER(X)	((X== '+') || (X=='-') || (X >= '0') && (X <= '9'))
#define	NUMBER(X)	((X >= '0') && (X <= '9'))
#define	LETTER(X)	(((X >= 'A') && (X <= 'Z')) || ((X >= 'a') && (X <= 'z')))
#define	FILL(X)		((X == ' ') || (X == '\t') || (X == '\n') || (X == '\r'))
#define	APPEND		*(fil++)= c;
#define	UN		ungetc(c, fu);
#define	APP0		*fil= '\0';

   /***
      simple parsing results
	 0  EOL
	 1  integer
	 2  float
	 3  word (only letters)
	 4  identifier (letters +numbers and _)
	 5  expanded identifier (filename, has a {./:} in it)
	 7  special character
	 -1  error
	 */

static int	scanner()
{
	char *fil;
	int c, state, substate;

	state= 0, fil= epart_hold;
	while( 1)
	{
		c= getc(fu);
		if (c == '\032') continue;
		if ((c == EOF) || (state && (state < 6) && FILL(c)))
			{ APP0  return state; }
		switch (state)
		{
			case 0:
				if FNUMBER(c) { state= 1;  APPEND  break; }
				if (c == '.') { state= 2, substate= 0;  APPEND  break; }
				if LETTER(c) { state= 3;  APPEND  break; }
				if ((c == '\\') || (c == '/') || (c == ':') || (c == '@')) 
					{ state= 5;  APPEND  break; }
				if (c == '#') { state= 8;  break; }
				if (c == '\'') { state= 9;  break; }
				if FILL(c) { break; }
				APPEND  APP0
				return 7;
			case 1:
				if FNUMBER(c) { APPEND  break; }
				if (c == '.') { state= 2, substate= 0;  APPEND  break; }
				if (c == 'e') { state= 2, substate= 1;  APPEND  break; }
				if LETTER(c)
				{
					if ('+' == *epart_hold) { UN  APP0  return 1; }
						else { state= 4;  APPEND  break; }
				}
				APP0  return -1;
			case 2:
				switch (substate)
				{
					case 0:
						if NUMBER(c) { APPEND  continue; }
						if (c == 'e') { substate= 1;  APPEND  continue; }
						break;
					case 1:
						if (NUMBER(c) || (c == '+') || (c == '-'))
						{ substate= 2;  APPEND  continue; }
						UN  c= '0', APPEND  APP0  return state;
					case 2:
						if NUMBER(c) { APPEND  continue; }
						break;
				}
				if LETTER(c) { UN  APP0  return state; }
				APP0  return -1;
			case 3:
				if LETTER(c) { APPEND  break; }
				if ((c == '_') || (c == '-') || NUMBER(c)) { state= 4;  APPEND  break; }
				if ((c == '\\') || (c == '/') || (c == ':') || (c == '.') || (c == '*'))
					{ state= 5;  APPEND  break; }
				if ('+' == c) { UN APP0  return state; }
				APP0  return -1;
			case 4:
				if ((c == '_') || (c == '-') || NUMBER(c) || LETTER(c)) 
					{ APPEND  break; }
				if ((c == '\\') || (c == '/') || (c == ':') || (c == '.') || (c == '*'))
					{ state= 5;  APPEND  break; }
				if ('+' == c) { UN APP0  return state; }
				APP0  return -1;
			case 5:
				if ((c == '_') || (c == '-') || (c == '\\') || (c == '/') 
					|| (c == ':') || (c == '.') || (c == '*') || NUMBER(c) || LETTER(c))
					{ APPEND  break; }
				if ('+' == c) { UN APP0  return state; }
				APP0  return -1;
			case 8:
				if ((c == '\n') || (c == '\r') ||(c == '\0')) state= 0;
				break;
			case 9:
				if (c == '\'') { APP0  return 5; }
				APPEND
				break;
		}
	}
}

int	epart_scannerline(char *cp, int l)
{
	int c, n, ll;
	char *p;

	for (ll= l-1, n= ll, p= cp, c= 0; n ; )
	{
		if ((c= getc(fu)) == EOF) break;
		while ((c == '\r') || (c == '\n'))
		{
			if (n == ll) { if ((c= getc(fu)) == EOF) return 0; }
				else { *p= '\0';  return 1; }
		}
		*(p++)= c, n--;
	}

	*(p++)= '\0';
	return (n!= ll);
}

#define HOLDIS(X, Y)	if (!strcmp(epart_hold, X)) { *d= Y;  break; }

/*
			HOLDIS("grid", _SCENE_GRID)  
			HOLDIS("hsl", _SCENE_HUV)
			HOLDIS("messages", _SCENE_MESSAGES)  
			HOLDIS("name", _SCENE_NAME)
			HOLDIS("names", _SCENE_NAMES)
			HOLDIS("nameform", _SCENE_NAMEFORM)
			HOLDIS("reversetime", _SCENE_REVERSE)  
			HOLDIS("rgb", _SCENE_RGB)  
			HOLDIS("tga", _SURF_TGA)
			HOLDIS("vid", _SURF_VID)
*/

int	scan_art(long *d)
{
	static int hn;

	if (!epart_holdc)
		{ epart_holdc ++, hn= scanner(); }
	*d= 0;
	if (hn != 3) { return hn; }

	switch (*epart_hold)
	{
		case 'I': HOLDIS("INF", _INF)  *d= 0;  break;
		case 'a':
			HOLDIS("active", _SCENE_ACTIVE)  
			HOLDIS("adapt", _SCENE_ADAPT)  
			HOLDIS("ambient", _AMBIENT)
			HOLDIS("aspect", _SCENE_ASPECT)  
			HOLDIS("all", _SURF_ALL)
			HOLDIS("alpha", _SURF_ALPHA)
			HOLDIS("alphalite", _SURF_ALPHALITE)
			HOLDIS("alphalight", _SURF_ALPHALITE)
			HOLDIS("alt", _SURF_ALT)
			HOLDIS("alternate", _SURF_ALT)
			*d= 0;  break;
		case 'b':
			HOLDIS("background", _SCENE_BACKGR)
			HOLDIS("beam", _SCENE_BEAM)
			HOLDIS("box", _SCENE_BOX)
			HOLDIS("bright", _SCENE_BRIGHT)
			HOLDIS("buffer", _SCENE_BUFFER)
			HOLDIS("buffertol", _SCENE_BUFTOL)
			HOLDIS("buffertolerance", _SCENE_BUFTOL)
			HOLDIS("bump", _SURF_BUMP)
			HOLDIS("bumpy", _SURF_BUMPY)
			*d= 0;  break;
		case 'c':
			HOLDIS("character", _SCENE_CHARACTER)
			HOLDIS("complex", _SCENE_COMPLEX)
			HOLDIS("cone", _SCENE_CONE)
			HOLDIS("cubic", _SURF_CUBIC)
			HOLDIS("cylnder", _SURF_CYLNDER)
			HOLDIS("color", _COLOR)
			*d= 0;  break;
		case 'd':
			HOLDIS("depth", _SCENE_DEPTH)
			HOLDIS("darkcolor", _SURF_DARKC)
			HOLDIS("diffuse", _SURF_DIFFUSE)
			*d= 0;  break;
		case 'e':
			HOLDIS("exec", _SCENE_EXEC)
			HOLDIS("edgerange", _SURF_EDGERANGE)
			HOLDIS("envir", _SURF_RENVIR)
			HOLDIS("expo", _SURF_EXPO)
			HOLDIS("end", _END)
			*d= 0;  break;
		case 'f':
			HOLDIS("falloff", _SCENE_FALLOFF)
			HOLDIS("filter", _SCENE_FILTER)
			HOLDIS("flat", _SCENE_FLAT)
			HOLDIS("flip", _SCENE_FLIP)
			HOLDIS("fov", _MOTION_FOV)
			HOLDIS("fog", _SCENE_FOG)
			HOLDIS("fogcolor", _SCENE_FOGCOLOR)
			HOLDIS("format", _SCENE_FORMAT)
			*d= 0;  break;
		case 'g':
			HOLDIS("gamma", _SCENE_GAMMA)  
			HOLDIS("gauss", _SCENE_GAUSS)  
			HOLDIS("gaussian", _SCENE_GAUSS)  
			HOLDIS("group", _SCENE_GRID)  
			HOLDIS("glass", _SURF_GLASS)
			HOLDIS("glassback", _SURF_GLASSBACK)
			HOLDIS("gloss", _SURF_GLOSS)
			HOLDIS("grain", _SURF_GRAIN)
			*d= 0;  break;
		case 'h':
			HOLDIS("horizon", _SCENE_HORIZON)
			*d= 0;  break;
		case 'i':
			HOLDIS("image", _SCENE_IMAGE)
			HOLDIS("import", _SCENE_IMPORT)
			HOLDIS("infsphere", _SCENE_INFSPH)
			HOLDIS("index", _SURF_INDEX)
			HOLDIS("invis", _INVIS)
			*d= 0;  break;
		case 'k':
			HOLDIS("ka", _SURF_KA)
			HOLDIS("kd", _SURF_KD)
			HOLDIS("kg", _SURF_KT)
			HOLDIS("kl", _SURF_KL)
			HOLDIS("kr", _SURF_KR)
			HOLDIS("ks", _SURF_KS)
			HOLDIS("kt", _SURF_KT)
			*d= 0;  break;
		case 'l':
			HOLDIS("lanczos", _SCENE_LANCZOS)
			HOLDIS("lens", _SCENE_LENS)
			HOLDIS("light", _SCENE_LIGHT)
			HOLDIS("linear", _SCENE_LINEAR)
			HOLDIS("lower", _SCENE_LOWER)
			HOLDIS("liquid", _SURF_LIQUID)
			HOLDIS("luminant", _SURF_LUMINANT)
			*d= 0;  break;
		case 'm':
			HOLDIS("map", _SURF_MAP)  
			HOLDIS("material", _SCENE_MATERIAL)  
			HOLDIS("morph", _SCENE_MORPH)  
			HOLDIS("motion", _MOTION)  
			HOLDIS("mainrange", _SURF_MAINRANGE)
			HOLDIS("marblesv", _SURF_MARBLESV)
			HOLDIS("mirror", _SURF_MIRROR)
			HOLDIS("mix", _SURF_MIX)
			HOLDIS("mixgloss", _SURF_MIXGLOSS)
			*d= 0;  break;
		case 'n':
			HOLDIS("nearfar", _SCENE_NEARFAR)
			HOLDIS("noisy", _SCENE_NOISY)
			HOLDIS("none", _SURF_NONE);
			HOLDIS("normal", _SURF_NORMAL);
			*d= 0;  break;
		case 'o':
			HOLDIS("ob", _SCENE_OB)
			HOLDIS("object", _SCENE_OBJECT)
			*d= 0;  break;
		case 'p':
			HOLDIS("parent", _SCENE_PARENT)
			HOLDIS("path", _SCENE_PATH)
			HOLDIS("picture", _SCENE_PICTURE)
			HOLDIS("post", _SCENE_POST)
			HOLDIS("position", _MOTION_POSITION)
			HOLDIS("power", _SCENE_POWER)
			HOLDIS("prepare", _PREPARE)
			HOLDIS("planar", _SURF_PLANAR)
			HOLDIS("project", _SURF_PROJECT)
			*d= 0;  break;
		case 'r':
			HOLDIS("real", _SCENE_REAL)  
			HOLDIS("reflected", _SCENE_REFLECTED)  
			HOLDIS("root", _SCENE_ROOT)
			HOLDIS("rotation", _MOTION_ROTATION)
			HOLDIS("reflect", _SURF_REFLECT)
			HOLDIS("rough", _SURF_ROUGH)
			*d= 0;  break;
		case 's':
			switch (epart_hold[1])
			{
				case 'a':
					HOLDIS("safe", _SAFE)
					*d= 0;  break;
				case 'c':
					HOLDIS("scale", _SCALE)
					*d= 0;  break;
				case 'e':
					HOLDIS("setup", _SCENE_SETUP)
					HOLDIS("seedcolor", _SURF_SEEDCOLOR)
					HOLDIS("serv", _SCENE_SERV)
					*d= 0;  break;
				case 'h':
					HOLDIS("shadow", _SCENE_SHADOW)
					HOLDIS("shades", _SURF_SHADES)
					*d= 0;  break;
				case 'i':
					HOLDIS("size", _SIZE)
					*d= 0;  break;
				case 'o':
					HOLDIS("soft", _SCENE_SOFT)
					HOLDIS("solid", _SOLID)
					*d= 0;  break;
				case 'p':
					HOLDIS("spherel", _SCENE_SPHEREL)
					HOLDIS("spot", _SCENE_SPOT)
					HOLDIS("spectral", _SURF_SPECULAR)
					HOLDIS("specular", _SURF_SPECULAR)
					HOLDIS("sphere", _SURF_SPHERE)
					*d= 0;  break;
				case 'q':
					HOLDIS("squarecyl", _SURF_SQUARECYL)
					*d= 0;  break;
				case 't':
					HOLDIS("stonesv", _SURF_STONESV)
					*d= 0;  break;
				case 'u':
					HOLDIS("surface", _SURFACE)
					*d= 0;  break;
				case 'w':
					HOLDIS("swirl", _SURF_SWIRL)
					HOLDIS("swirlfreq", _SURF_SWIRLFREQ)
					*d= 0;  break;
				case 'y':
					HOLDIS("sync", _SCENE_LANCZOS)
				default:
					*d= 0;  break;
			}
			break;
		case 't':
			HOLDIS("target", _MOTION_TARGET)  
			HOLDIS("tracks", _SCENE_TRACKS)  
			HOLDIS("tri", _SCENE_TRI)  
			HOLDIS("triangle", _SCENE_TRI)  
			HOLDIS("texture", _SURF_TEXTURE)
			HOLDIS("transmit", _SURF_TRANSMIT)
			HOLDIS("turb", _SURF_TURB)
			*d= 0;  break;
		case 'u':
			HOLDIS("up", _MOTION_UP)
			HOLDIS("upper", _SCENE_UPPER)
			HOLDIS("use", _SCENE_USE)
			*d= 0;  break;
		case 'v':
			HOLDIS("view", _SCENE_VIEW)
			HOLDIS("veinrange", _SURF_VEINRANGE)
			*d= 0;  break;
		case 'w':
			HOLDIS("warpview", _SCENE_WARPVIEW)
			HOLDIS("wideview", _SCENE_WIDEVIEW)
			HOLDIS("window", _SCENE_WINDOW)
			HOLDIS("world", _SCENE_WORLD)
			HOLDIS("woodsv", _SURF_WOODSV)
			HOLDIS("wrap", _SURF_WRAP)
			*d= 0;  break;
		case 'q':  HOLDIS("q", _MOTION_Q)  *d= 0;  break;
		case 'z':  HOLDIS("zone", _SCENE_ZONE)  *d= 0;  break;
		default:
			*d= 0;  break;
	}
	return 3;
}

	/****/

int		open_art(char *finame)
{
		epart_holdc= 0;
		fu= fopen(finame, "r");
		return (NULL == fu);
}
void	rewind_art(void)
		{ epart_holdc= 0;  if (NULL != fu) rewind(fu); }
void	close_art(void)
		{ epart_holdc= 0;  if (NULL != fu) fclose(fu); }

	/****/

FILE	* getFile(void) { return fu ; }

