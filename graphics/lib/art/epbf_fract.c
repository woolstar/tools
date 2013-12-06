#include "basic.h"
#include "ebf.h"

#define	WFRA_TEST 0

unsigned int	eft_codes, eft_arrayd= 0, eft_sizespace,
	*f_space= NULL, *f_final= NULL;
bf_flat	*x_initial= NULL;

int	fract(void);

   /***/

static char	ixpress[SLINE];
static unsigned int	*ffp, *ffx;

static bf_flat	*x_work, *x_cave;
static unsigned int	f_index, 
		ef_code, ef_size, ef_holes,
		*x_index, *x_try,
		*x_codes, *x_cive;
static float	ef_toler;

static void	eftest_add(), final_face();
static int	eftest_try(), eftest_start(),
			eftest_cross(), effind_reach(),
			efstack_add(), efadjust_back(), efadjust_across(),
			efstack_plus(), efadjust_forward();

static void	zink(char *ee)
			{ printf("processing objects: %s\n", ee); }

/*
   /// i don't want to be doing this
   /// but it is a key part of my technology and value
*/

int	mem_fract(int n1, int n2, int n3)
{
	x_initial= EMEM((3 * n1), bf_flat),
	x_index= EMEM((4 * n1 + n2 + n3), unsigned int);

	if ((x_initial == NULL) || (x_index == NULL))
	{
		long ems;

		ems= (3 *n1) *sizeof(bf_flat) +(4*n1 +n2 +n3) * sizeof(unsigned int);
		printf(" >> failed allocating %s for fracture structures [%d %d %d]",
			mem_name(ems), n1, n2, n3);
		EFREE(x_initial); EUFREE(x_index);
		RE
	}

	x_work= x_initial +n1, x_cave= x_initial +(2*n1);
	x_try= x_index + n1,
	x_codes= x_index + (2*n1),
	x_cive= x_index + (3*n1),
	f_space= x_index + (4*n1);
	f_final= f_space +n2;

	eft_arrayd= n1,
	eft_sizespace= n2;
	return 0;
}

/*****/
/* /// edge tests */

   /***
      test for self crossing problems by looking for multiple
      inside corners that are above the outside neighbors.
      */

static int	eftest_cross(int a, int b)
{
	unsigned int
		i,
		abv, faultct;
	float
		dx, dy, 
		adx, ady, lm, lb;
	bf_flat *vik;

	/* find the slope of the line between the segment start and end point */
	faultct= 0;
	dx= x_work[b].x -x_work[a].x, dy= x_work[b].y -x_work[a].y,
	adx= (dx >= 0.0) ? dx : -dx, ady= (dy >= 0.0) ? dy : -dy;
	if ((adx < 1e-10) && (ady < 1e-10)) return 0;
	if (adx > ady)
	{
		/* find the slope and x intercept */
		lm= dy / dx, lb= x_work[b].y -lm * x_work[b].x;
		abv= (dx < 0.0);

		/* check that all inbetween points are below the line */
		for (i= a+1, vik= x_work +i; (i!= b); vik++, i++)
		{
			if (i == ef_size) { i= 0, vik= x_work;  if (!b) break; }
			dx= lm * vik-> x -vik-> y +lb;
			if ((dx == 0.0) || ((dx > 0.0) != abv)) { faultct++; }
		}
	}
	else
	{
		/* find the slope and x intercept */
		lm= dx / dy, lb= x_work[b].x -lm * x_work[b].y;
		abv= (dy > 0.0);

		/* check that all inbetween points are below the line */
		for (i= a+1, vik= x_work +i; (i!= b); vik++, i++)
		{
			if (i == ef_size) { i= 0, vik= x_work;  if (!b) break; }
			dx= lm * vik-> y -vik-> x +lb;
			if ((dx == 0.0) || ((dx > 0.0) != abv)) { faultct++; }
		}
	}

	return (faultct > 1);
}

static int	eftest_start(int a, int b)
{
	unsigned int
		i, j, abv,
		*vfi_fil, *cv_step;
	float
		dx, dy, 
		adx, ady,
		lm, ln, lb;
	bf_flat
		*vik, *vfu_fil;

	/* find the slope of the line between the curent start and end point */
	dx= x_work[b].x -x_work[a].x, dy= x_work[b].y -x_work[a].y,
		adx= (dx >= 0.0) ? dx : -dx, ady= (dy >= 0.0) ? dy : -dy;

	/* deal with the line in whichever axis is larger (so we don't divide by zero) */
	if (adx > ady)
	{
			/* find the slope and x intercept */
		if (dx < 0) { lm= -dy, ln= -dx; } else { lm= dy, ln= dx; }
		lb= ln * x_work[b].y -lm * x_work[b].x;
		abv= (dx < 0.0);

			/* keep the holes that are inside the current edge */
		vik= vfu_fil= x_cave, vfi_fil= cv_step= x_cive;
		for (i= ef_holes, j= 0; (i--); vik++, cv_step++)
		{
			if (*cv_step == a) continue;
			dx= lm * vik-> x -ln * vik-> y +lb;
			if ((dx!= 0.0) && ((dx > 0.0) == abv))
			{ 
				if (vik != vfu_fil) { *vfu_fil= *vik, *vfi_fil= *cv_step; }
				vfu_fil++, vfi_fil++, j++; 
			}
		}
	}
	else
	{
			/* find the slope and y intercept */
		if (dy < 0) { lm= -dx, ln= -dy; } else { lm= dx, ln= dy; }
		lb= ln * x_work[b].x -lm * x_work[b].y;
		abv= (dy > 0);

			/* keep the holes that are inside the current edge */
		vik= vfu_fil= x_cave, vfi_fil= cv_step= x_cive;
		for (i= ef_holes, j= 0; (i--); vik++, cv_step++)
		{
			if (*cv_step == a) continue;
			dx= lm * vik-> y - ln * vik-> x +lb;
			if ((dx!= 0.0) && ((dx > 0.0) == abv))
			{ 
				if (vik != vfu_fil) { *vfu_fil= *vik, *vfi_fil= *cv_step; }
				vfu_fil++, vfi_fil++, j++; 
			}
		}
	}

	ef_holes= j;
	return 0;
}

static int	eftest_try(int a, int b, int c, unsigned int *x)
{
	unsigned int
		i, j, ji,
		ab, abv1, abv2,
		*cvst;
	float
		dtmp, dx, dy, 
		adx, ady, 
		lm1, ln1, lb1,
		lm2, ln2, lb2;
	bf_flat
		*fust, ful, fur;

/* /// composite function now
		check all points
		if (one, or best of many) return in x, and 1
		if (many and no best) return many
	*/

	ab= 0;
	ful= x_work[a], fur= x_work[b];
/*	/// screen out near colinear sets first */
	dx= (ful.x -fur.x) * (x_work[c].y -fur.y),
	dy= (x_work[c].x -fur.x) * (ful.y -fur.y), adx= (dx > dy) ? dx-dy : dy-dx;
	if (adx < 1e-12) 
		return 0;

	dx= x_work[c].x -ful.x, dy= x_work[c].y -ful.y,
		adx= (dx < 0.0) ? -dx : dx, ady= (dy < 0.0) ? -dy : dy;
	if (adx > ady)
	{
		adx= dx;
		if (dx < 0) { lm1= -dy, ln1= -dx; } else { lm1= dy, ln1= dx; }
		lb1= ln1 * ful.y -lm1 * ful.x;
		dx= lm1 *fur.x, dy= ln1 * fur.y -lb1;
		abv1= (dx > dy);
		if ((adx > 0) != abv1) { zink("numerical accuracy test failed check 1x"); }
	}
	else
	{
		ady= dy;
		if (dy < 0) { lm1= -dx, ln1= -dy; } else { lm1= dx, ln1= dy; }
		lb1= ln1 * ful.x -lm1 * ful.y;
		dx= lm1 *fur.y, dy= ln1 * fur.x -lb1;
		abv1= (dx > dy), ab++;
		if ((ady < 0) != abv1) { zink("numerical accuracy test failed check 1y"); }
	}

	dx= x_work[c].x -fur.x, dy= x_work[c].y -fur.y,
		adx= (dx < 0.0) ? -dx : dx, ady= (dy < 0.0) ? -dy : dy;
	if (adx > ady)
	{
		adx= dx;
		if (dx < 0) { lm2= -dy, ln2= -dx; } else { lm2= dy, ln2= dx; }
		lb2= ln2 * fur.y -lm2 * fur.x;
		dx= lm2 *ful.x, dy= ln2 * ful.y -lb2;
		abv2= (dx > dy);
		if ((adx < 0) != abv2) { zink("numerical accuracy test failed check 2x"); }
	}
	else
	{
		ady= dy;
		if (dy < 0) { lm2= -dx, ln2= -dy; } else { lm2= dx, ln2= dy; }
		lb2= ln2 * fur.x -lm2 * fur.y;
		dx= lm2 *ful.y, dy= ln2 * ful.x -lb2;
		abv2= (dx > dy), ab+= 2;
		if ((ady > 0) != abv2) { zink("numerical accuracy test failed check 2y"); }
	}

	for (i= ef_holes, j= 0, fust= x_cave, cvst= x_cive; 
		(i--); fust++, cvst++)
	{
		switch (ab)
		{
			case 0:
				dx= lm1 * fust-> x - ln1 * fust-> y +lb1;
					if ((dx == 0.0) || ((dx > 0.0) != abv1)) continue ;
				dx= lm2 * fust-> x - ln2 * fust-> y +lb2;
					if ((dx == 0.0) || ((dx > 0.0) != abv2)) continue ;
				break;
			case 1:
				dx= lm1 * fust-> y - ln1 * fust-> x +lb1;
					if ((dx == 0.0) || ((dx > 0.0) != abv1)) continue ;
				dx= lm2 * fust-> x - ln2 * fust-> y +lb2;
					if ((dx == 0.0) || ((dx > 0.0) != abv2)) continue ;
				break;
			case 2:
				dx= lm1 * fust-> x - ln1 * fust-> y +lb1;
					if ((dx == 0.0) || ((dx > 0.0) != abv1)) continue ;
				dx= lm2 * fust-> y - ln2 * fust-> x +lb2;
					if ((dx == 0.0) || ((dx > 0.0) != abv2)) continue ;
				break;
			case 3:
				dx= lm1 * fust-> y - ln1 * fust-> x +lb1;
					if ((dx == 0.0) || ((dx > 0.0) != abv1)) continue ;
				dx= lm2 * fust-> y - ln2 * fust-> x +lb2;
					if ((dx == 0.0) || ((dx > 0.0) != abv2)) continue ;
				break;
		}

		if (x == NULL) RE
		dtmp= fust-> x -ful.x, adx= dtmp*dtmp,
		dtmp= fust-> y -ful.y, adx+= dtmp*dtmp;
		dtmp= fust-> x -fur.x, adx+= dtmp*dtmp,
		dtmp= fust-> y -fur.y, adx+= dtmp*dtmp;
		if (j) { if (adx < ady) { ji= *cvst, ady= adx; }}
			else { j= 1, ji= *cvst, ady= adx; }
	}

	if (j)
	{
		*x= ji;
		return (eftest_try(a,b, (int) ji, (unsigned int *) NULL)) ? 2 : 1;
	}

	return 0;
}

static void	eftest_add(int b, int c)
{
	unsigned int
		i, j, abv,
		*cvst, *cvfl;
	float
		dx, dy, 
		adx, ady,
		lm, ln, lb;
	bf_flat
		*fust, *fufl;

	/* find the slope of the line between the curent start and end point */
	dx= x_work[c].x -x_work[b].x, dy= x_work[c].y -x_work[b].y,
		adx= (dx >= 0.0) ? dx : -dx, ady= (dy >= 0.0) ? dy : -dy;

	if (adx > ady)
	{
			/* find the slope and x intercept */
		if (dx < 0) { lm= -dy, ln= -dx; } else { lm= dy, ln= dx; }
		lb= ln * x_work[b].y -lm * x_work[b].x;
		abv= (dx < 0);

			/* keep the holes that are inside the current edge */
		fust= fufl= x_cave, cvst= cvfl= x_cive;
		for (i= ef_holes, j= 0; (i--); fust++, cvst++)
		{
			dx= lm * fust-> x - ln * fust-> y +lb;
			if ((dx!= 0.0) && ((dx > 0.0) == abv))
			{ 
				if (fust != fufl) { *fufl= *fust, *cvfl= *cvst; }
				fufl++, cvfl++, j++; 
			}
		}
	}
	else
	{
			/* find the slope and y intercept */
		if (dy < 0) { lm= -dx, ln= -dy; } else { lm= dx, ln= dy; }
		lb= ln * x_work[b].x -lm * x_work[b].y;
		abv= (dy > 0);

			/* keep the holes that are inside the current edge */
		fust= fufl= x_cave, cvfl= cvst= x_cive;
		for (i= ef_holes, j= 0; (i--); fust++, cvst++)
		{
			dx= lm * fust-> y - ln * fust-> x +lb;
			if ((dx!= 0.0) && ((dx > 0.0) == abv))
			{ 
				if (fust != fufl) { *fufl= *fust, *cvfl= *cvst; }
				fufl++, cvfl++, j++; 
			}
		}
	}

	ef_holes= j;
}

	/*****/
	/* /// adjustment routines */

static int	efadjust_across(int a, int b, int n)
{
	unsigned int i, j;
	float dtmp, dd1, dd2;
	bf_flat lu_base, *vek;

	lu_base= x_work[b], vek= x_work +a;
	dtmp= vek-> x -lu_base.x, dd1= dtmp*dtmp,
	dtmp= vek-> y -lu_base.y, dd1+= dtmp*dtmp, dd1*= ef_toler;

	for (i= a, j= a+1, vek= x_work +j; (n > 3); j++, vek++, n--)
	{
		if (j == ef_size) { j= 0, vek= x_work; }
		dtmp= vek-> x -lu_base.x, dd2= dtmp*dtmp;
		dtmp= vek-> y -lu_base.y, dd2+= dtmp*dtmp;
		if (dd2 > dd1) break;
		dd1= dd2, i= j;
	}

	return (int) i;
}

static int	efadjust_forward(int a, int b, int n)
{
	unsigned int i, j;
	double dtmp, dd1, dd2;
	bf_flat
		lu_base, lu_vir, *vek;

	i= (b) ? (b-1) : (ef_size-1);
	lu_base= x_work[b], vek= x_work +i;
	lu_vir.x= vek-> x -lu_base.x, lu_vir.y= vek-> y -lu_base.y;

	for (i= a, j= a, vek= x_work +a, dd1= 1e300; (n > 2); j++, vek++, n--)
	{
		if (j == ef_size) { j= 0, vek= x_work; }
		dtmp= (vek-> x -lu_base.x) * lu_vir.x + (vek-> y -lu_base.y) * lu_vir.y; 
		dd2= dtmp * dtmp;  if (dd2 > dd1) break;
		dd1= dd2, i= j;
	}

	return (int) i;
}

static int	efadjust_back(int a, int b, int n)
{
	unsigned int i, j;
	double dtmp, dd1, dd2;
	bf_flat
		lu_base, lu_vir, *vek;

	i= (a+1) % ef_size;
	lu_base= x_work[a], vek= x_work +i;
	lu_vir.x= vek-> x -lu_base.x, lu_vir.y= vek-> y -lu_base.y;

	for (i= b, j= b, vek= x_work +b, dd1= 1e300; (n > 2); j--, vek--, n--)
	{
		dtmp= (vek-> x -lu_base.x) * lu_vir.x + (vek-> y -lu_base.y) * lu_vir.y; 
		dd2= dtmp * dtmp;  if (dd2 > dd1) break;
		dd1= dd2, i= j;
		if (!j) { j= ef_size, vek= x_work +ef_size; }
	}

	return (int) i;
}

static int	effind_reach(int a, int b)
{
	unsigned int i;
	int j;
	float
		dtmp, dsum,
		dbst;
	bf_flat
		cor1, cor2,
		*vik;

	cor1= x_work[a], cor2= x_work[b];
	for (i= a+1, j= -1, vik= x_work +i, dbst= 0.0; (i!= b); i++)
	{
		if (i == ef_size) { i= 0, vik= x_work;  if (!b) break; }
		dtmp= vik-> x -cor1.x, dsum= dtmp * dtmp,
		dtmp= vik-> y -cor1.y, dsum+= dtmp * dtmp,
		dtmp= vik-> x -cor2.x, dsum+= dtmp * dtmp,
		dtmp= vik-> y -cor2.y, dsum+= dtmp * dtmp;
		if ((j == -1) || (dsum < dbst)) { dbst= dsum, j= i; }
	}

	return (j!= -1) ? j : 0;
}

	/*****/
	/* /// stack routines */

static int	efstack_add(int a, int b, int x)
{
	unsigned int i, j,
		*efi_step, *ef_fill;

	j= f_index, ef_fill= f_space +f_index;
	if (x)
	{
		for (efi_step= x_index +b, i= b; (1); efi_step--, i--)
		{
			*(ef_fill--)= *efi_step, j--;  if (i == a) break;
			if (!i) { i= ef_size, efi_step= x_index +ef_size; }
		}
	}
	else
	{
		for (efi_step= x_index +a, i= a; (1); efi_step++, i++)
		{
			if (i== ef_size) { i= 0, efi_step= x_index; }
			*(ef_fill--)= *efi_step, j--;  if (i == b) break;
		}
	}
	if (ef_fill <= ffp)
		{ printf(" >> out of workspace for new fracture pieces\n");  return 0; }

	*(ffp++)= f_index | (x ^ ef_code), *ef_fill= 0;
	j--, f_index= j;
	return 0;
}

static int	efstack_plus(int a, int b, int c)
{
	unsigned int
		i, j,
		*efi_step, *ef_fill;

	j= f_index, ef_fill= f_space +f_index;
	for (efi_step= x_index +a, i= a; (1); efi_step++, i++)
	{
		if (i== ef_size) { i= 0, efi_step= x_index; }
		*(ef_fill--)= *efi_step, j--;
		if (i == b) break;
	}
	efi_step= x_index +c, *(ef_fill--)= *efi_step, j--;

	if (ef_fill <= ffp)
		{ printf(" >> out of workspace for new fracture pieces\n");  return 0; }

	*(ffp++)= f_index | ef_code, *ef_fill= 0; 
	j--, f_index= j;
	return 0;
}

static void	final_face(unsigned int *ulist)
{
	unsigned int i, j;
	unsigned int
		*p1, *p2,
		pfir, pshf;

	ef_toler= 1.5;

	if (ef_code)
	{
		if (eft_codes)
		{
			p1= ulist, pfir= *(p1++), pshf= *(p1++);
			if (!pfir || !pshf) return;
			for (i= 4094, p2= ffx; (i && *p1); p1++, i--)
				{ *(p2++)= pshf, *(p2++)= pfir, *(p2++)= pshf= *p1, *(p2++)= 0; }
			if (i) ffx= p2;
			return;
		}

		for (i= 4096, j= 0, p1= ulist; (i && *p1); p1++, j++, i--) ;
		for (i= j, p2= ffx; (i--); p2++) { *p2= *(--p1); }
		if (!i) return;
	}
	else
	{
		if (eft_codes)
		{
			p1= ulist, pfir= *(p1++), pshf= *(p1++);
			if (!pfir || !pshf) return;
			for (i= 4094, p2= ffx; (i && *p1); p1++, i--)
				{ *(p2++)= pfir, *(p2++)= pshf, *(p2++)= pshf= *p1, *(p2++)= 0; }
			if (i) ffx= p2;
			return;
		}

		for (i= 4096, p1= ulist, p2= ffx; (i && *p1); p1++, p2++, i--)
			{ *p2= *p1; }
		if (!i) return;
	}
	ffx= p2, *(ffx++)= 0;
}

int	fract(void)
{
	short ef_sz;
	unsigned int
		i, j,
		ef_ecc,
		ef_start, ef_end, ef_check,
		*st_step, *st_cpy;
	bf_flat
		*fu_base, 
		*fu_point, *fu_fill, ho_cent, ho_hi,
		*fu_l, *fu_r, fe_left, fe_new;
	float dtmp;

	if (f_space == NULL)
		{ printf(" >> fracture not initialized\n");  RE }

	ffp= f_space+1, ffx= f_final, fu_base= x_initial -1;
	ef_code= 0, ef_ecc= 0, ef_toler= 1.5;

		/*** scan through in pointer for all poly pieces to operate on */
	while (ffp!= f_space)
	{
			/*** get size of next piece, load piece to x_index */
		i= *(--ffp), ef_code= i&0x8000, i&= 0x7fff;
		st_step= f_space +i, f_index= i;
		for (st_cpy= x_index, ef_size= 0; (i= *(st_step--)); ef_size++, st_cpy++) *st_cpy= i;

			/*** evaulate piece for concave sections */
		st_step= x_index, fu_point= fu_base +(*(st_step++));
		ho_cent= ho_hi= *fu_point;
		for (i= ef_size -1; (i--); st_step++)
		{
			fu_point= fu_base +(*st_step);
			dtmp= fu_point-> x;
			if (dtmp < ho_cent.x) ho_cent.x= dtmp;
				else if (dtmp > ho_hi.x) ho_hi.x= dtmp;
			dtmp= fu_point-> y;
			if (dtmp < ho_cent.y) ho_cent.y= dtmp;
				else if (dtmp > ho_hi.y) ho_hi.y= dtmp;
		}
		dtmp= -0.5 * (ho_cent.x + ho_hi.x), ho_cent.x= dtmp;
		dtmp= -0.5 * (ho_cent.y + ho_hi.y), ho_cent.y= dtmp;
		for (fu_fill= x_work, st_step= x_index, i= ef_size; (i--); fu_fill++, st_step++) 
		{
			fu_point= fu_base +(*st_step);
			FPp_add(ho_hi, fu_point, ho_cent)
			if (ef_code) { fu_fill-> x= ho_hi.y, fu_fill-> y= ho_hi.x; }
				else { *fu_fill= ho_hi; }
		}
		ef_holes= 0;
		if (ef_size > 3)
		{
			for (i= ef_size, j= 0, st_step= x_codes, fu_fill= x_cave, st_cpy= x_cive,
					fu_point= x_work, fu_r= x_work +1, fu_l= x_work +i -1;
				(i--); st_step++, j++, fu_l= fu_point, fu_point= fu_r, fu_r++)
			{
				if (!i) fu_r= x_work;
				dtmp= (fu_r-> x - fu_point-> x) * (fu_l-> y - fu_point-> y)
					- (fu_r-> y - fu_point-> y) * (fu_l-> x - fu_point-> x);
				if (dtmp < -1e-16) 
						{ ef_holes++, *st_step= 1, *(fu_fill++)= *fu_point, *(st_cpy++)= j; }
					else *st_step= 0;
			}
		}

		if (!ef_holes)
		{
			/*****
				Well, we just got hit by the [H] factor.  When you cut
				up a rectangle, you get some co-linear points, which
				have to be removed, or sector triangulation will result
				in some null segments, which will die horribly.

				DW  5-31-91
			*/

			fu_point= x_work, fu_r= x_work +1, fu_l= x_work +ef_size -1;
			for (i= ef_size, j= 0, st_step= x_index, st_cpy= x_index;
				(i--); fu_l= fu_point, fu_point= fu_r, fu_r++, st_step++)
			{
				if (!i) fu_r= x_work;
				dtmp= (fu_r-> x - fu_point-> x) * (fu_l-> y - fu_point-> y)
					- (fu_r-> y - fu_point-> y) * (fu_l-> x - fu_point-> x);
				if ((dtmp < -1e-12) || (dtmp > 1e-12))
					{ *(st_cpy++)= *st_step, j++; }
			}

				/** sign off on face and continue */
			ef_ecc= 0;
			*st_cpy= 0;
			if (j > 2) final_face(x_index);
			continue;
		}

			/*** test for illegal face */
		if (ef_holes > (ef_size -3)) continue;
		if (ef_ecc > 32) return 1;

/*    	////// half way polygon masaging
			/// find first outside corner
			/// scan for groups of interior points (2 or more)
			/// test that the group is inside its bounds
			/// if not, throw out points and push the rest on, and restart

			after some thought, and continued artistic escapades outsides the bounds
			of well behavied polygons, i have given in to the drive to success and
			am implementing a routine to test and remove self crossing edges
			but it only works for point single cross cases, i need to think of
			something better for other situations
			DW 3-8-93
*/

		for (i= ef_size, j= 0, st_step= x_codes; (i); i--, j++, st_step++)
			{ if (!*st_step) break; }
		if (!i) { printf(" >> breakdown reducing polygon, no outside edges\n");  RE }

		ef_end= j, ef_sz= 0;
		for (i= ef_size+1; i; )
		{
			for (ef_start= ef_end; (i && (!*st_step)); )
			{
				ef_start= ef_end, ef_end++, st_step++, i--;
				if (ef_end == ef_size) { ef_end= 0, st_step= x_codes; }
			}
			for (ef_sz= 1; i; ef_sz++)
			{
				ef_end++, st_step++, i--;
				if (ef_end == ef_size) { ef_end= 0, st_step= x_codes; }
				if (!*st_step) break;
			}
			if (i && (ef_sz == 2) && eftest_cross((int) ef_start, (int) ef_end))
			{
				ef_check= effind_reach((int) ef_start, (int) ef_end);
				efstack_plus((int) ef_end, (int) ef_start, (int) ef_check);
				ef_sz= -1;
				break;
			}
		}
		if (ef_sz == -1) continue;

			/*    /// Just like good old times, lets go split  */
		for (i= ef_size-1, j= 0, st_step= x_codes, st_cpy= x_codes+1; 
			(i); i--, j++, st_step++, st_cpy++) { if (*st_step && !(*st_cpy)) break; }
		if (!i && x_codes[0])
			{ printf(" >> breakdown subdividing polygon, no inside corner found.\n"); RE }
		ef_start= j, ef_end= (i) ? j+1 : 0, ef_sz= 2;
		if (eftest_start((int) ef_start, (int) ef_end))
		{
			ef_ecc++, ef_end= (ef_end+1) % ef_size;
			if (efstack_add((int) ef_end, (int) ef_start, 0)) return 1;
#if WFRA_TEST
				NEISY ("l");
#endif
			continue ;
		}

		ef_check= (ef_end +1) % ef_size; 
		fu_point= x_work +ef_start, 
		fu_l= x_work +ef_end,
		fu_r= (ef_check) ? fu_l +1 : x_work;

		fe_left.x= fu_l-> x -fu_point-> x, fe_left.y= fu_l-> y -fu_point-> y;

			/** look for cutting point from starting point */
		while (ef_check != ef_start)
		{
			fe_new.x= fu_r-> x -fu_point-> x, fe_new.y= fu_r-> y -fu_point-> y;
			dtmp= (fe_new.y * fe_left.x) - (fe_new.x * fe_left.y);
			if ((dtmp < 0.0) && (ef_sz != 2))
			{
				/*** current slice is larger than 180 degrees, backup and cut here */

				i= efadjust_back((int) ef_start, (int) ef_end, (int) ef_sz);
				if (efstack_add((int) i, (int) ef_start, 0) 
					|| efstack_add((int) ef_start, (int) i, 0)) return 1;
#if WFRA_TEST
				NEISY ("x");
#endif
				break;
			}

			if (i= eftest_try((int) ef_start, (int) ef_end, (int) ef_check, &j))
			{
				if (i == 1)
				{
					/*** (w)
						one point inside current setup
						backup and wicker cut to it
					*/

					ef_ecc= 0;
					ef_check= j, ef_sz++;
					i= efadjust_back((int) ef_check, (int) ef_end, (int) ef_sz), ef_end= i;
					ef_sz= ef_end -ef_start;  if (ef_sz < 0) ef_sz+= ef_size;  ef_sz+= 2;
					i= efadjust_across((int) ef_start, (int) ef_check, ef_sz), ef_start= i;

					if ( efstack_add((int) ef_end, (int) ef_check, 0x8000) ||
							efstack_add((int) ef_check, (int) ef_start, 0) ||
							efstack_plus((int) ef_start, (int) ef_end, (int) ef_check))
						{ return 1; }
#if WFRA_TEST
					NEISY("w");
#endif
					break;
				}

				if (ef_sz == 2)
				{
					ef_toler*= 0.96, ef_ecc++;
					ef_start= (ef_end+1) % ef_size;
					if (efstack_add((int) ef_start, (int) ef_end, (ef_ecc & 4) ? 0x8000 : 0))
						return 1;
					break;
				}

				/*** (v)
					don't wicker because there are too many inside points
					backup and make a cut after a certain threshold is reached

					there are notes about massive failures with blatent backup cutting
					so there are tests to put off cutting to a certain tolerance
				*/

				i= efadjust_back((int) ef_start, (int) ef_end, (int) ef_sz);
				if ((i == ef_end) && (ef_toler > 1.0))
				{
/*			      			/// avoid backing up for now, but step tolerances */
					ef_toler*= 0.97, ef_ecc++,
					ef_start= (ef_end+1) % ef_size;
					if (efstack_add((int) ef_start, (int) ef_end, (ef_ecc & 4) ? 0x8000 : 0))
						return 1;
					break;
				}

				ef_end= i;
				if (efstack_add((int) ef_end, (int) ef_start, 0)
					|| efstack_add((int) ef_start, (int) ef_end, 0)) return 1;
#if WFRA_TEST
				NEISY("v");
#endif
				break;
			}

			if (x_codes[ef_check])
			{
				/*** (u)
					cut across two concave points

					NOTES from pon/pre
					for some reason once apon a time, there was a bunch of code
					with tolerances and cross products, to inhibit cross cuts
					until other solutions had been exhausted to avoid slivers
					but now its not a problem so im not concerned.

					DW 5-3-92 (from DW 4-28-90 and 5-30-90)
				*/

				ef_ecc= 0;
				ef_end= ef_check, ef_sz++;
				ef_check= efadjust_back((int) ef_start, (int) ef_end, (int) ef_sz);
				if (ef_check == ef_end)
				{
					ef_check= efadjust_forward((int) ef_start, (int) ef_end, (int) ef_sz);
					ef_start= ef_check;
				} 
					else ef_end= ef_check;
				if (efstack_add((int) ef_end, (int) ef_start, 0) 
					|| efstack_add((int) ef_start, (int) ef_end, 0)) RE
#if WFRA_TEST
				NEISY("u");
#endif
				break;
			}

/*			/// add check point */
			eftest_add((int) ef_end, (int) ef_check);
			ef_end= ef_check, fu_point= fu_r;
			ef_check++, fu_r++, ef_sz++;
			if (ef_check == ef_size) { ef_check= 0, fu_r= x_work; }
		}
	}

	*ffx= 0;
	return 0;
}

