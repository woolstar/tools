#include	"rrun.h"

	/** _camera */
extern	tr_camera	*curcam;

	/** dx_mempage */
extern svector	*workspace;
extern dx_point	*workpoint;

extern int			global_time;

	/** l_thread.c */
#if	TRENDER_THREAD
	extern int	p_active ;

#	define	PTEST_ACTIVE	if (! p_active) return
#else
#	define	PTEST_ACTIVE	if (0) return
#endif

	#define	TEST	0

#if TEST
	FILE 	*ftest = NULL;

void	teststart(void)
{
	static int	aseq= 1;
	char namm[32] ;

	sprintf(namm, "\\temp\\l%03d", aseq ++) ;
	ftest= fopen(namm, "w");
}
void	testend()
{
	fclose(ftest);
	ftest= NULL;
}

	int	testone= 0, testtwo= 0 ;

#endif

void	gcam_projectpts(tr_object *opcr)
{
	long istep;
	vector
		*vstep,
		vtmp, voff;
	svector *pfill;

	/*** replace lbound, hbound with Mc space */

	voff= curcam-> basev;
	for (istep= opcr-> ta.vector_c, vstep= opcr-> ta.vlist, pfill= workspace;
		(istep --); vstep ++, pfill ++)
	{
		vtmp= *vstep;

		pfill-> x= M_dot(vtmp, curcam-> cside) - voff.x,
		pfill-> y= M_dot(vtmp, curcam-> ctop) - voff.y,
		pfill-> z= - M_dot(vtmp, curcam-> cview) + voff.z;
	}
}

#define	IfN	if (kfaceatt & RACE_NORMAL)
#define IfT	if (kfaceatt & RACE_TEXTURE)

#define	CLIP_NEAR	0.125

int	gcam_clips(int n, er_face *faceptr, tr_object *opcr)
{
	int
		istep, jstep, icount, jcount,
		vx, vy, vlx, vhx, vly, vhy;
	short
		viewlx, viewhx, viewly, viewhy,
		pailx, paily, paihx, paihy;
	shurt
		isout, issout, isfirout,
		kfaceatt, kfaceside, kfacecheck,
		*invptr, *innptr, *inuptr,
		indl, *ik;
	double
		dtmp, dvid, du, dau,
		tsx, tsy, tox, toy;
	vector
		*vbase, *vptr;
	svector
		*sptr,
		*pbase, *nbase;
	dx_point
		*dpbase, *dpfill, *dstep,
		dvcur, dvfol, dvfir;

	pbase= workspace -1,
	vbase= opcr-> ta.vlist -1,
	nbase= opcr-> ta.nlist -1;
	dpbase= workpoint;

	tsx= curcam-> scale_x, tsy= curcam-> scale_y,
	tox= curcam-> offset_x, toy= curcam-> offset_y;
	viewlx= curcam-> view_ixl, viewhx= curcam-> view_ixr,
	viewly= curcam-> view_iyb, viewhy= curcam-> view_iyt -1;

	for (istep= n, icount= 0; (istep--); faceptr ++)
	{
		PTEST_ACTIVE -1;
		
		kfaceatt= faceptr-> bits;
		if (! kfaceatt) continue ;

		kfaceside= kfaceatt & 0x3ff;
		ik= faceptr-> indic_sides;
			/*** trivial z clipping */
		for (invptr= ik, kfacecheck= kfaceside;
			(kfacecheck && (pbase[*invptr].z < 1e-10)); kfacecheck --, invptr ++) ;
		if (!kfacecheck) continue;
			/*** later: possible XY trivial cull, accept */

		invptr= ik, ik += kfaceside;
		IfN { innptr= ik, ik += kfaceside; }
		IfT { inuptr= ik, ik += (kfaceside << 1); }

		dpfill= dpbase, jstep= kfaceside, jcount= 0;
		indl= *(invptr ++), sptr= pbase + indl, dvcur.p= *sptr;
		vptr= vbase +indl, dvcur.v= *vptr;
		IfN { indl= *(innptr ++), sptr= nbase +indl, dvcur.n= *sptr; }
		IfT
		{
			dtmp= *(inuptr ++), dvcur.t.x= 0.0000305176 * dtmp,
			dtmp= *(inuptr ++), dvcur.t.y= 0.0000305176 * dtmp,
			dvcur.t.z= 0.0;
		}
		if (dvcur.p.z > 1e-10) { *(dpfill ++)= dvcur, isout= 0, jcount ++; } else isout= 1;
		dvfir= dvcur, isfirout= isout;
		for (jstep --; (jstep --); )
		{
			issout= isout, dvfol= dvcur;
			indl= *(invptr ++), sptr= pbase + indl, dvcur.p= *sptr;
			vptr= vbase +indl, dvcur.v= *vptr;
			IfN { indl= *(innptr ++), sptr= nbase +indl, dvcur.n= *sptr; }
			IfT
			{
				dtmp= *(inuptr ++), dvcur.t.x= 0.0000305176 * dtmp,
				dtmp= *(inuptr ++), dvcur.t.y= 0.0000305176 * dtmp,
				dvcur.t.z= 0.0;
			}
			isout= !(dvcur.p.z > 1e-10);

			if (issout != isout)
			{
					/** clip */
				dtmp= dvcur.p.z - dvfol.p.z;
				if ((dtmp > 1e-8) || (dtmp < -1e-8))
				{
					du= (CLIP_NEAR -dvfol.p.z) / dtmp, dau= 1.0 -du;
					if ((du > 0.0) && (du < 1.0))
					{
						M_add(dpfill-> p, dau * dvfol.p, du * dvcur.p)
						M_add(dpfill-> v, dau * dvfol.v, du * dvcur.v)
						IfN { M_add(dpfill-> n, dau * dvfol.n, du * dvcur.n) }
						IfT { M_add(dpfill-> t, dau * dvfol.t, du * dvcur.t) }
						/* M_add(dpfill-> c, dau * dvfol.c, du * dvcur.c) */
						dpfill ++, jcount ++;
					}
				}
			}
			if (!isout) { *(dpfill ++)= dvcur, jcount ++; }
		}

		if (isfirout != isout)
		{
				/** clip */
			dtmp= dvfir.p.z - dvcur.p.z;
			if ((dtmp > 1e-8) || (dtmp < -1e-8))
			{
				du= (CLIP_NEAR -dvcur.p.z) / dtmp, dau= 1.0 -du;
				if ((dtmp > 1e-8) || (dtmp < -1e-8))
				{
					M_add(dpfill-> p, dau * dvcur.p, du * dvfir.p)
					M_add(dpfill-> v, dau * dvcur.v, du * dvfir.v)
					IfN { M_add(dpfill-> n, dau * dvcur.n, du * dvfir.n) }
					IfT { M_add(dpfill-> t, dau * dvcur.t, du * dvfir.t) }
					/* M_add(dpfill-> c, dau * dvcur.c, du * dvfir.c) */
					dpfill ++, jcount ++;
				}
			}
		}

		if (jcount < 3) continue;
		for (jstep= jcount, dstep= dpbase; (jstep --); dstep ++)
		{
			dstep-> ltipe= 0;
			dvid= 1.0 / dstep-> p.z;
			dtmp= dstep-> p.x * dvid * tsx + tox, vx= dstep-> p.x= dtmp;
			dtmp= dstep-> p.y * dvid * tsy + toy, vy= dstep-> p.y= dtmp;
			if (dstep == dpbase) { vlx= vhx= vx, vly= vhy= vy;  continue; }
			if (vx < vlx) vlx= vx; else if (vx > vhx) vhx= vx;
			if (vy < vly) vly= vy; else if (vy > vhy) vhy= vy;
		}
		if ((vhx < viewlx) || (vlx > viewhx) || (vhy < viewly) || (vly > viewhy)
				|| (vlx == vhx) || (vly == vhy)) continue;

		PTEST_ACTIVE -1;
		gtile_poly(jcount, faceptr);

		if (vlx < viewlx) { vlx= viewlx; }
		if (vhx > viewhx) { vhx= viewhx; }
		if (vly < viewly) { vly= viewly; }
		if (vhy > viewhy) { vhy= viewhy; }
		if (!icount) { pailx= vlx, paihx=vhx, paily= vly, paihy= vhy; }
		else
		{
			if (vlx < pailx) pailx= vlx;
			if (vhx > paihx) paihx= vhx;
			if (vly < paily) paily= vly;
			if (vhy > paihy) paihy= vhy;
		}

#if 0
		curcam-> paint_lx= vlx,
		curcam-> paint_ly= vly,
		curcam-> paint_hx= vhx,
		curcam-> paint_hy= vhy;
		gscan_check();
#endif

		icount ++;
	}

	if (icount)
	{
		if (pailx < curcam-> paint_lx) curcam-> paint_lx= pailx;
		if (paihx > curcam-> paint_hx) curcam-> paint_hx= paihx;
		if (paily < curcam-> paint_ly) curcam-> paint_ly= paily;
		if (paihy > curcam-> paint_hy) curcam-> paint_hy= paihy;
	}
	return icount;
}

int	gdepth_clips(int n, er_face *faceptr, tr_object *opcr)
{
	int
		istep, jstep, icount, jcount,
		vx, vy, vlx, vhx, vly, vhy;
	short
		viewlx, viewhx, viewly, viewhy,
		pailx, paily, paihx, paihy;
	shurt
		isout, issout, isfirout,
		kfaceatt, kfaceside, kfacecheck,
		*invptr, indl, *ik;
	double
		dtmp, dvid, du, dau,
		tsx, tsy, tox, toy;
	vector
		*vbase, *vptr;
	svector
		*sptr,
		*pbase;
	dx_point
		*dpbase, *dpfill, *dstep,
		dvcur, dvfol, dvfir;

	pbase= workspace -1,
	vbase= opcr-> ta.vlist -1;

	dpbase= workpoint;

	tsx= curcam-> scale_x, tsy= curcam-> scale_y,
	tox= curcam-> offset_x, toy= curcam-> offset_y;
	viewlx= curcam-> view_ixl, viewhx= curcam-> view_ixr,
	viewly= curcam-> view_iyb, viewhy= curcam-> view_iyt -1;

	for (istep= n, icount= 0; (istep--); faceptr ++)
	{
		kfaceatt= faceptr-> bits;
		if (kfaceatt & RACE_INVIS) continue;

		kfaceside= kfaceatt & 0x3ff;
		ik= faceptr-> indic_sides;
			/*** trivial z clipping */
		for (invptr= ik, kfacecheck= kfaceside;
			(kfacecheck && (pbase[*invptr].z < 1e-10)); kfacecheck --, invptr ++) ;
		if (!kfacecheck) continue;
			/*** later: possible XY trivial cull, accept */

		invptr= ik, ik += kfaceside;

		dpfill= dpbase, jstep= kfaceside, jcount= 0;
		indl= *(invptr ++), sptr= pbase + indl, dvcur.p= *sptr;
		vptr= vbase +indl, dvcur.v= *vptr;
		if (dvcur.p.z > 1e-10) { *(dpfill ++)= dvcur, isout= 0, jcount ++; } else isout= 1;
		dvfir= dvcur, isfirout= isout;
		for (jstep --; (jstep --); )
		{
			issout= isout, dvfol= dvcur;
			indl= *(invptr ++), sptr= pbase + indl, dvcur.p= *sptr;
			vptr= vbase +indl, dvcur.v= *vptr;
			isout= !(dvcur.p.z > 1e-10);

			if (issout != isout)
			{
					/** clip */
				dtmp= dvcur.p.z - dvfol.p.z;
				if ((dtmp > 1e-8) || (dtmp < -1e-8))
				{
					du= (CLIP_NEAR -dvfol.p.z) / dtmp, dau= 1.0 -du;
					if ((du > 0.0) && (du < 1.0))
					{
						M_add(dpfill-> p, dau * dvfol.p, du * dvcur.p)
						M_add(dpfill-> v, dau * dvfol.v, du * dvcur.v)
						dpfill ++, jcount ++;
					}
				}
			}
			if (!isout) { *(dpfill ++)= dvcur, jcount ++; }
		}

		if (isfirout != isout)
		{
				/** clip */
			dtmp= dvfir.p.z - dvcur.p.z;
			if ((dtmp > 1e-8) || (dtmp < -1e-8))
			{
				du= (CLIP_NEAR -dvcur.p.z) / dtmp, dau= 1.0 -du;
				if ((dtmp > 1e-8) || (dtmp < -1e-8))
				{
					M_add(dpfill-> p, dau * dvcur.p, du * dvfir.p)
					M_add(dpfill-> v, dau * dvcur.v, du * dvfir.v)
					dpfill ++, jcount ++;
				}
			}
		}

		if (jcount < 3) continue;

		if (curcam-> cfov > 0.1)
		{
			for (jstep= jcount, dstep= dpbase; (jstep --); dstep ++)
			{
				dstep-> ltipe= 0;
				dvid= 1.0 / dstep-> p.z;
				dtmp= dstep-> p.x * dvid * tsx + tox, vx= dstep-> p.x= dtmp;
				dtmp= dstep-> p.y * dvid * tsy + toy, vy= dstep-> p.y= dtmp;
				if (dstep == dpbase)
					{ vlx= vhx= vx, vly= vhy= vy;  continue; }
				if (vx < vlx) vlx= vx; else if (vx > vhx) vhx= vx;
				if (vy < vly) vly= vy; else if (vy > vhy) vhy= vy;
			}
			if ((vhx < viewlx) || (vlx > viewhx) || (vhy < viewly) || (vly > viewhy)
					|| (vlx == vhx) || (vly == vhy)) continue;
			gdepth_poly(jcount, faceptr);
		}
		else
		{
			for (jstep= jcount, dstep= dpbase; (jstep --); dstep ++)
			{
				dstep-> ltipe= 0;
				dtmp= dstep-> p.x * tsx + tox, vx= dstep-> p.x= dtmp;
				dtmp= dstep-> p.y * tsy + toy, vy= dstep-> p.y= dtmp;
				if (dstep == dpbase)
					{ vlx= vhx= vx, vly= vhy= vy;  continue; }
				if (vx < vlx) vlx= vx; else if (vx > vhx) vhx= vx;
				if (vy < vly) vly= vy; else if (vy > vhy) vhy= vy;
			}
			if ((vhx < viewlx) || (vlx > viewhx) || (vhy < viewly) || (vly > viewhy)
					|| (vlx == vhx) || (vly == vhy)) continue;
			gdepthortho_poly(jcount, faceptr);
		}

		if (vlx < viewlx) { vlx= viewlx; }
		if (vhx > viewhx) { vhx= viewhx; }
		if (vly < viewly) { vly= viewly; }
		if (vhy > viewhy) { vhy= viewhy; }
		if (!icount) { pailx= vlx, paihx=vhx, paily= vly, paihy= vhy; }
		else
		{
			if (vlx < pailx) pailx= vlx;
			if (vhx > paihx) paihx= vhx;
			if (vly < paily) paily= vly;
			if (vhy > paihy) paihy= vhy;
		}

#if 0
		curcam-> paint_lx= vlx,
		curcam-> paint_ly= vly,
		curcam-> paint_hx= vhx,
		curcam-> paint_hy= vhy;
		gscan_check();
#endif

		icount ++;
	}

	if (icount)
	{
		if (pailx < curcam-> paint_lx) curcam-> paint_lx= pailx;
		if (paihx > curcam-> paint_hx) curcam-> paint_hx= paihx;
		if (paily < curcam-> paint_ly) curcam-> paint_ly= paily;
		if (paihy > curcam-> paint_hy) curcam-> paint_hy= paihy;
	}
	return icount;
}

typedef struct
{
	float lx, ly, hx, hy;
} cambound;

#define	CBtest(ix, iy, C) ((ix < C.lx) || (iy < C.ly) || (ix > C.hx) || (iy > C.hy))

static void	clipln(float ax, float ay, float bx, float by, cambound * c,
	char * ref, void (* draw)(char *, int, int, int, int))
{
	float dtmp, nx, ny;

	if (ax < c-> lx) /**/
	{
		nx= bx - ax, ny= by - ay;
		if (bx < c-> lx) return ;
		dtmp= ((c-> lx - ax) / nx);
		ax= c-> lx, ay += dtmp * ny;
	}
	if (bx < c-> lx) /**/
	{
		nx= bx - ax, ny= by - ay;
		dtmp= ((c-> lx - bx) / nx);
		bx= c-> lx, by += dtmp * ny;
	}

	if (ay < c-> ly)
	{
		nx= bx - ax, ny= by - ay;
		if (by < c-> ly) return ;
		dtmp= ((c-> ly - ay) / ny);
		ax+= dtmp * nx, ay= c-> ly;
	}
	if (by < c-> ly)
	{
		nx= bx - ax, ny= by - ay;
		dtmp= ((c-> ly - by) / ny);
		bx+= dtmp * nx, by = c-> ly;
	}

	if (ax > c-> hx)
	{
		nx= bx - ax, ny= by - ay;
		if (bx > c-> hx) return ;
		dtmp= ((c-> hx - ax) / nx);
		ax= c-> hx, ay += dtmp * ny;
	}
	if (bx > c-> hx)
	{
		nx= bx - ax, ny= by - ay;
		dtmp= ((c-> hx - bx) / nx);
		bx= c-> hx, by += dtmp * ny;
	}

	if (ay > c-> hy) /**/
	{
		nx= bx - ax, ny= by - ay;
		if (by > c-> hy) return ;
		dtmp= ((c-> hy - ay) / ny);
		ax+= dtmp * nx, ay= c-> hy;
	}
	if (by > c-> hy)
	{
		nx= bx - ax, ny= by - ay;
		dtmp= ((c-> hy - by) / ny);
		bx+= dtmp * nx, by = c-> hy;
	}

	(* draw)(ref, ax, ay, bx, by);
}

int	gcam_lineclip(int n, er_face *faceptr, tr_object *opcr,
	char * ref, void (* drawln)(char *, int, int, int, int))
{
	int istep, jstep;
	shurt
		isout, issout, isfirout,
		isclip, issclip, isfirclip,
		kfaceatt, kfaceside, kfacecheck,
		*invptr, indl, *ik;
	float
		dtmp, dvid, du, dau,
		tsx, tsy, tox, toy,
		px, py, pox, poy, pfx, pfy;

	cambound camf;
	svector
		*pbase,
		svcur, svfol, svfir, svint;

	pbase= workspace -1;

	tsx= curcam-> scale_x, tsy= - curcam-> scale_y,
	tox= curcam-> offset_x, toy= curcam-> offset_y;
	camf.lx= curcam-> view_ixl, camf.hx= curcam-> view_ixr,
	camf.ly= curcam-> view_iyb, camf.hy= curcam-> view_iyt -1;

	for (istep= n; (istep--); faceptr ++)
	{
		kfaceatt= faceptr-> bits;
		if (kfaceatt & RACE_INVIS) continue;

		kfaceside= kfaceatt & 0x3ff;
		ik= faceptr-> indic_sides;
			/*** trivial z clipping */
		for (invptr= ik, kfacecheck= kfaceside;
			(kfacecheck && (pbase[*invptr].z < 1e-10)); kfacecheck --, invptr ++) ;
		if (!kfacecheck) continue;
			/*** later: possible XY trivial cull, accept */

		invptr= ik, ik += kfaceside;

		jstep= kfaceside;
			indl= *(invptr ++), svcur= pbase[indl];
		if (svcur.z > 1e-10)
		{
			dvid= 1.0 / svcur.z;
			dtmp= svcur.x * dvid * tsx + tox, px= dtmp;
			dtmp= svcur.y * dvid * tsy + toy, py= dtmp;
			isout= 0;

			isclip= CBtest(px, py, camf);
		}
			else isout= 1, isclip= 1;

		svfir= svcur, isfirout= isout;
		pfx= px, pfy= py;
		isfirclip= isclip;

		for (jstep --; (jstep --); )
		{
			issout= isout, issclip= isclip;
			svfol= svcur, pox= px, poy= py;

			indl= *(invptr ++), svcur= pbase[indl];
			isout= !(svcur.z > 1e-10);

			if (! isout)
			{
				dvid= 1.0 / svcur.z;
				px= svcur.x * dvid * tsx + tox;
				py= svcur.y * dvid * tsy + toy;
			}

			if (issout != isout)
			{
					/** clip */
				dtmp= svcur.z - svfol.z;
				if ((dtmp > 1e-8) || (dtmp < -1e-8))
				{
					du= (CLIP_NEAR -svfol.z) / dtmp, dau= 1.0 -du;
					if ((du > 0.0) && (du < 1.0))
						{ M_add(svint, dau * svfol, du * svcur) }
				}

				dvid= 1.0 / svint.z;
				if (isout)
				{
					px= svint.x * dvid * tsx + tox;
					py= svint.y * dvid * tsy + toy;

				}
				else
				{
					pox= svint.x * dvid * tsx + tox;
					poy= svint.y * dvid * tsy + toy;
					issclip= CBtest(pox, poy, camf);
				}
			}
				else if (isout) continue ;

			isclip= CBtest(px, py, camf);
			if (! isclip && ! issclip) { (* drawln)(ref, pox, poy, px, py); }
				else { clipln(pox, poy, px, py, & camf, ref, drawln); }

		}

		if (isfirout != isout)
		{
				/** clip */
			dtmp= svfir.z - svcur.z;
			if ((dtmp > 1e-8) || (dtmp < -1e-8))
			{
				du= (CLIP_NEAR -svcur.z) / dtmp, dau= 1.0 -du;
				if ((du > 0.0) && (du < 1.0))
					{ M_add(svint, dau * svcur, du * svfir) }
			}
		}

		if (! isclip && ! isfirclip) { (* drawln)(ref, px, py, pfx, pfy); }
			else { clipln(px, py, pfx, pfy, & camf, ref, drawln); }
	}

	return 0;
}

