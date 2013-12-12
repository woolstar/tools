
typedef struct
{
	int ltipe;
	vector v;
	svector p, n, t;
} dx_point;

typedef struct DXedge
{
	long lwindbits;
	dx_point *plead, *pfollow;
	double
		ystart, yfall,
		vk, vstep,
		iz0, diz, bz, ivz,
		ir0, dir, br, ivr;
	svector
		kp, dp,
		v0, kv, dv,
		n0, kn, dn,
		t0, kt, dt,
		c0, kc, dc;
	struct DXedge *pair;
} dx_edge;

typedef struct
{
	int vx, vy, dx, dy;
	svector
		camx, camy, camz,
		camo, cama;
	double
		mx, my, ox, oy,
		zpersp,
		znear, zfar, zscale;
} wp_camera_txformdat;

typedef struct
{
	int adapt, shaderate;
	unsigned long *zbuf;
	dx_edge *edgelist, *edgefar;
} wp_buffer;

typedef struct
{
	svector
		space_p,
		space_d,
		hit_p,
		hit_n,
		hit_t;
	color color;
} wp_ray;

