
typedef struct	LIGHTscope
{
	long	isurfattr;
	colU	lit, lightshade, darkshade, propd, props;
	vector	skinnorm, viewdir;
	er_surface	*skinsurf;
} cl_scope ;
