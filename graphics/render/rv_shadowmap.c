#include "rrun.h"

extern void		tgeom_solidrange(void), tgeom_depth(void) ;

static void	rvm_writeim(int, int, unsigned long *) ;
int	rvm_shadowcolumn(ev_light *, int, int),
	rvm_shadowcube(ev_light *, int, int),
	rvm_shadowbuffer(ev_light *, int, int) ;

extern tr_camera 	tmpcam, *curcam;
extern long			cobjects;
extern tr_object	*tolist;

static void	rvm_writeim(int x, int y, unsigned long *lp)
{
	int istep, jstep, ival;
	ul *lrow, *lstep;
	static byte bhd[18];
	char imname[64];
	FILE *f;
	static int	testimct= 1;

	memset(bhd, 0, 18);
	bhd[2]= 2;
	bhd[12]= 0xff & x,
	bhd[13]= 0xff & (x >> 8),
	bhd[14]= 0xff & y,
	bhd[15]= 0xff & (y >> 8);
	bhd[16]= 24;
	bhd[17]= 0x20;

	sprintf(imname, "dpth%d.tga", testimct ++);
	f= fopen(imname, "w");
	if (NULL == f) return;

	fwrite((char *) bhd, 18, 1, f);

#if 0
	for (istep= x * y; (istep --); lp ++)
	{
		ival= *lp >> 20;
		if (ival > 255) { ival= 255; }
		bhd[0]= bhd[1]= bhd[2]= 255 - ival;
		fwrite((char *) bhd, 3, 1, f);
	}
#else
	lrow= lp + x*y;
	for (istep= y; (istep --); )
	{
		lrow -= x;
		for (jstep= x, lstep= lrow; (jstep --); lstep ++)
		{
			ival= *lstep >> 20;
			if (ival > 255) { ival= 255; }
			bhd[0]= bhd[1]= bhd[2]= 255 - ival;
			fwrite((char *) bhd, 3, 1, f);
		}
	}
#endif

	fclose(f);
}

int	rvm_shadowcolumn(ev_light *ptr, int x, int y)
{
	unsigned int
		istep, jstep,
		shbufx, shbufy;
	long l, lstep;
	ul *dplate, *dfinal,
		*ulstep, *ulstap, *ulfill;
	double r, dtmp;
	vector
		ldir, lu, lv,
		lmin, lmax,
		*vpt;
	ev_lightshadowbuffer *elbuf;
	tr_object *tstep;

	tmpcam.cfov= 0.0;

	shbufx= x, shbufy= y;
	l= shbufx * shbufy;
	dplate= EMEM((2 * l), ul);
	if (NULL == dplate)
		return 1;

	memset((char *) dplate, 0xff, 2 * l * sizeof(ul));

	elbuf= EMEM(1, ev_lightshadowbuffer);
	if (NULL == elbuf) 
		{ return 1; }

	dfinal= EMEM(l, ul);
	if (NULL == dfinal)
		{ return 1; }

	elbuf-> attrib= 0,
	elbuf-> prepx= shbufx,
	elbuf-> prepy= shbufy,
	elbuf-> plast= 1;
	elbuf-> buffer= dfinal;
	ptr-> lshadmap= elbuf;

	tmpcam.page_width= shbufx,
	tmpcam.page_size= l;
	tmpcam.pixbuf= NULL,
	tmpcam.depthbuf= dplate,
	tmpcam.backbuf= dplate +l;

	tmpcam.view_ixl= 0, tmpcam.view_ixr= shbufx,
	tmpcam.view_iyb= 0, tmpcam.view_iyt= shbufy;
	tmpcam.offset_x= 0.5 * (1 + shbufx),
	tmpcam.offset_y= 0.5 * (1 + shbufy);

	M_zero(lv)
	M_cpy(ldir, ptr-> direc)
	if (ldir.y > 0.75) lv.z= 1.0;
		else if (ldir.y < -0.75) lv.z= -1.0;
		else lv.y= 1.0;
	M_cross(lu, lv, ldir)  M_selfnorm(lu)
	M_cross(lv, lu, ldir)

	M_cpy(elbuf-> vcore, ldir)
	M_cpy(elbuf-> vplanei, lu)
	M_cpy(elbuf-> vplanej, lv)
	M_zero(tmpcam.cpos)
	M_cpy(tmpcam.cview, ldir)
	M_cpy(tmpcam.cside, lu)
	M_cpy(tmpcam.ctop, lv)

	lmin.x= lmin.y= lmin.z= 1e304,
	lmax.x= lmax.y= lmax.z= -1e304;
	for (istep= cobjects, tstep= tolist; (istep --); tstep ++)
	{
		if ((tstep-> ia != 3) || (tstep-> attr & 0x10)) continue;

		/* only do body */
		for (jstep= tstep-> ta.vector_c, vpt= tstep-> ta.vlist; (jstep --); vpt ++)
		{
			r= vpt-> x * lu.x + vpt-> y * lu.y + vpt-> z * lu.z;
				if (r < lmin.x) lmin.x= r;  if (r > lmax.x) lmax.x= r;
			r= vpt-> x * lv.x + vpt-> y * lv.y + vpt-> z * lv.z;
				if (r < lmin.y) lmin.y= r;  if (r > lmax.y) lmax.y= r;
			r= - (vpt-> x * ldir.x + vpt-> y * ldir.y + vpt-> z * ldir.z);
				if (r < lmin.z) lmin.z= r;  if (r > lmax.z) lmax.z= r;
		}
	}

	/*
	printf("column:  %g - %g by %g - %g, depth %g to %g\n",
		lmin.x, lmax.x, lmin.y, lmax.y, lmin.z, lmax.z);
	*/

	tmpcam.basev.x= 0.5 * (lmin.x + lmax.x),
	tmpcam.basev.y= 0.5 * (lmin.y + lmax.y),
	tmpcam.basev.z= 0.0;

	dtmp= lmax.x - lmin.x;
	if (dtmp > 1e-10) tmpcam.scale_x= 1.0 * shbufx / dtmp;
		else tmpcam.scale_x= 0.0;
	dtmp= lmax.y - lmin.y;
	if (dtmp > 1e-10) tmpcam.scale_y= 1.0 * shbufy / dtmp;
		else tmpcam.scale_y= 0.0;

	if (lmin.z < 1e-3)
	{
		tmpcam.basev.z= 1e-3 - lmin.z;
		lmin.z += tmpcam.basev.z,
		lmax.z += tmpcam.basev.z;
	}

	dtmp= lmax.z - lmin.z;
	if (dtmp < 1e-10) dtmp= 1e-10;
	tmpcam.offset_z= lmin.z,
	tmpcam.scale_z= 267175328.0 / dtmp;

	elbuf-> safepad= tmpcam.scale_z * ptr-> safedist;

	M_cpy(elbuf-> vbase, tmpcam.basev)
	elbuf-> wscale.x= tmpcam.scale_x,
	elbuf-> wscale.y= tmpcam.scale_y,
	elbuf-> wscale.z= tmpcam.scale_z,
	elbuf-> woffset.x= tmpcam.offset_x,
	elbuf-> woffset.y= tmpcam.offset_y,
	elbuf-> woffset.z= tmpcam.offset_z;

	curcam= & tmpcam;
	tgeom_depth();
	/* rvm_writeim(shbufx, shbufy, dplate); */

	for (lstep= l, ulstep= tmpcam.depthbuf, ulstap= tmpcam.backbuf, ulfill= dfinal;
		(lstep --); ulstep ++, ulstap ++, ulfill ++)
	{
		if (0xffffffff == *ulstap) { *ulfill= *ulstap;  continue; }
		*ulfill= (*ulstep + *ulstap) / 2;
	}

	ptr-> bits |= ERLA_BUFFER;
	/* rvm_writeim(shbufx, shbufy, dfinal); */
	EFREE(dplate);

	return 0;
}

int	rvm_shadowcube(ev_light *ptr, int x, int y)
{
	/*
	*/

	return 1;
}

int	rvm_shadowbuffer(ev_light *ptr, int x, int y)
{
	unsigned int
		shbufx, shbufy;
	double r, dtmp;
	vector
		ldir, la, lu, lv, lt;
	long l, lstep;
	ul *dplate, *dfinal,
		*ulstep, *ulstap, *ulfill;
	ev_lightshadowbuffer *elbuf;

	dtmp= ptr-> cone_edge;
	if (dtmp > 0.0) { r= 200.0 / M_PI * acos(dtmp); }
		else r= 90.0;
	tmpcam.cfov= r;

	shbufx= x, shbufy= y;
	l= shbufx * shbufy;
	dplate= EMEM((2 * l), ul);
	if (NULL == dplate)
		return 1;
	memset((char *) dplate, 0xff, 2 * l * sizeof(ul));

	elbuf= EMEM(1, ev_lightshadowbuffer);
	if (NULL == elbuf) 
		{ return 1; }

	dfinal= EMEM(l, ul);
	if (NULL == dfinal)
		{ return 1; }

	elbuf-> attrib= 4,
	elbuf-> prepx= shbufx,
	elbuf-> prepy= shbufy,
	elbuf-> plast= 1;
	elbuf-> buffer= dfinal;
	elbuf-> safepad= 0;

	ptr-> lshadmap= elbuf;

	tmpcam.page_width= shbufx,
	tmpcam.page_size= l;
	tmpcam.pixbuf= NULL,
	tmpcam.depthbuf= dplate,
	tmpcam.backbuf= dplate +l;
	tmpcam.view_ixl= 0, tmpcam.view_ixr= shbufx,
	tmpcam.view_iyb= 0, tmpcam.view_iyt= shbufy;
	tmpcam.offset_x= 0.5 * (1 + shbufx),
	tmpcam.offset_y= 0.5 * (1 + shbufy);

	dtmp= 1.0 / tan(M_PI * r / 180.0);
	tmpcam.scale_x= 0.5 * dtmp * x,
	tmpcam.scale_y= 0.5 * dtmp * y;

	M_cpy(la, ptr-> pt)
	M_cpy(ldir, ptr-> direc)
	M_zero(lv)
	if (ldir.y > 0.75) lv.z= 1.0;
		else if (ldir.y < -0.75) lv.z= -1.0;
		else lv.y= 1.0;
	M_cross(lu, lv, ldir)  M_selfnorm(lu)
	M_cross(lv, lu, ldir)

	M_neg(ldir)
	M_cpy(elbuf-> vcore, ldir)
	M_cpy(elbuf-> vplanei, lu)
	M_cpy(elbuf-> vplanej, lv)
	M_cpy(tmpcam.cpos, la)
	M_cpy(tmpcam.cview, ldir)
	M_cpy(tmpcam.cside, lu)
	M_cpy(tmpcam.ctop, lv)

	lt.x= M_dot(la, lu),
	lt.y= M_dot(la, lv),
	lt.z= M_dot(la, ldir);
	M_cpy(tmpcam.basev, lt)

	tmpcam.page_width= x,
	tmpcam.page_size= x * y;
	tmpcam.pixbuf= NULL,
	tmpcam.depthbuf= dplate,
	tmpcam.backbuf= dplate +l;

	curcam= & tmpcam;
	tgeom_solidrange();

	M_cpy(elbuf-> vbase, tmpcam.basev)
	elbuf-> wscale.x= tmpcam.scale_x,
	elbuf-> wscale.y= tmpcam.scale_y,
	elbuf-> wscale.z= tmpcam.scale_z,
	elbuf-> woffset.x= tmpcam.offset_x,
	elbuf-> woffset.y= tmpcam.offset_y,
	elbuf-> woffset.z= tmpcam.offset_z;

	tgeom_depth();
	/* rvm_writeim(shbufx, shbufy, dplate); */

	for (lstep= l, ulstep= tmpcam.depthbuf, ulstap= tmpcam.backbuf, ulfill= dfinal;
		(lstep --); ulstep ++, ulstap ++, ulfill ++)
	{
		if (0xffffffff == *ulstap) { *ulfill= *ulstap;  continue; }
		*ulfill= (*ulstep + *ulstap) / 2;
	}

	ptr-> bits |= ERLA_BUFFER;
	/* rvm_writeim(shbufx, shbufy, dfinal); */
	EFREE(dplate);

	return 0;
}
