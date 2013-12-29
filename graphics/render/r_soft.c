#include "rrun.h"

extern ray_element	*current;

extern colU		opaq_spf;

int	c_soft(ev_light *evptr, ev_lightshadowbuffer *p, int x, int y, ul z)
{
	unsigned int istep, jstep;
	int
		ival, 
		w, wx, wy, ox, oy,
		ctdark, ctlight;
	ul *lrow, *lstep, lz;
	double dv;

	w= evptr-> samples;
	ox= x -w, oy= y -w,
	wx= 1 + 2 * w, wy = 1 + 2 * w;
	ctdark= 0, ctlight= 0;

	while (wx && (ox < 0)) { ox ++, ctlight += wy, wx --; }
		if (! wx) return 0;
	ival= ox + wx +1;
	while (wx && (ival > p-> prepx)) { wx --, ctlight += wy; }
		if (! wx) return 0;

	while (wy && (oy < 0)) { oy ++, ctlight += wx, wy --; }
		if (! wy) return 0;
	ival= oy + wy +1;
	while (wy && (ival > p-> prepy)) { wy --, ctlight += wx; }
		if (! wy) return 0;

	lrow= p-> buffer + (oy * p-> prepx) + ox;
	for (istep= wy; (istep --); lrow += p-> prepx)
	{
		for (jstep= wx, lstep= lrow; (jstep --); lstep ++)
		{
			lz= *lrow;
			if (z < lz) ctlight ++; else ctdark ++;
		}
	}

	if (! ctlight) return 1;

	dv= 1.0 * ctlight / (ctlight + ctdark);
	opaq_spf.r= opaq_spf.g= opaq_spf.b= dv;
	return 0;
}

int		c_depth(ev_light *evptr)
{
	int ix, iy;
	unsigned long
		iz, oz,
		*lpos;
	double px, py, pz, dtmp;
	vector targp;
	ev_lightshadowbuffer *p;

	C_set(opaq_spf)

	p= evptr-> lshadmap;
		if (! p) return 0;
	targp= current-> hit_p;

	px= M_dot(targp, p-> vplanei) - p-> vbase.x,
	py= M_dot(targp, p-> vplanej) - p-> vbase.y;
	pz= - M_dot(targp, p-> vcore) + p-> vbase.z;

	if (p-> attrib & 4)
	{
		dtmp= (pz > 1e-10) ? 1.0 / pz : 0.0;
		ix= px * dtmp * p-> wscale.x + p-> woffset.x,
		iy= py * dtmp * p-> wscale.y + p-> woffset.y;

		if ((ix < 0) || (iy < 0) || (ix >= p-> prepx) || (iy >= p-> prepy))
			return 0;
	}
	else
	{
		ix= px * p-> wscale.x + p-> woffset.x,
		iy= py * p-> wscale.y + p-> woffset.y;
	}
	iz= p-> wscale.z * (pz - p-> woffset.z);
	if (iz > p-> safepad) iz -= p-> safepad; else iz= 0;

	if ((ix < 0) || (iy < 0))
	{
		if (ix < -1) { printf(" >> xrange error -\n"); }
		if (iy < -1) { printf(" >> yrange error -\n"); }
		return 0;
	}
	if ((ix >= p-> prepx) || (iy >= p-> prepy))
	{
		if (ix > p-> prepx) { printf(" >> xrange error +\n"); }
		if (iy > p-> prepy) { printf(" >> yrange error +\n"); }
		return 0;
	}

	if (evptr-> bits & ERLA_SOFT)
	{
		return c_soft(evptr, p, ix, iy, iz);
	}

	lpos= p-> buffer + (p-> prepx * iy) + ix;
	oz= *lpos;

	return (iz >= oz);
}

