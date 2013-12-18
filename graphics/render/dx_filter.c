#include "rrun.h"
#include "efilter.h"

static int attr_filttype= FILTER_BOX;
static short 	*ebase_xkernal, *ebase_ykernal, **ecur_yfiltertable;
static shurt 	ef_scanx= 1, ef_scany= 1, ef_ycount= 0,
				ef_filtsub= 1, ef_filtwin= 0, ef_span, ef_cpan,
				ef_radius= 0, ef_gap, ef_half, ef_odd,
				efbox_prefix= 0, efbox_shift= 0;
static sampI	*ebase_yset, *ecur_yset;
static long		ef_scanwidth;

#define	XSPAN	256
#define YSPAN	4096
#define	CapDw(xx)	if (xx < 0) xx= 0; else if (xx > 0x7fff) xx= 0x7ff0;

void	ei_genbox(int x, int subs, short *efiltbase)
{
	unsigned short i, j;
	short *efil_step;
	double dval, dint;

		/** quietly fix 0 subsamples per pixel booboo */
	if (subs == 0) subs = 1;

	ef_scanx= x,
	ef_filtsub= subs;
	ef_span= subs;
	ef_scanwidth = ((long) ef_scanx)*ef_filtsub;

	ebase_xkernal= efiltbase;
	if (ef_span < 2) *ebase_xkernal= 1;
	else
	{
			/** no need to multiply for box */
		for (i= ef_filtsub, j= 0; (!(i & 1)); j++)
			{ i >>= 1; }
		if (i & 0xfffe)
		{
			efbox_prefix= 1;
			efil_step= ebase_xkernal, *(efil_step ++)= 0;
			dval= 128.0 / (ef_filtsub * ef_filtsub);
			for (i= XSPAN -1, dint= dval; (i --); dint += dval) *(efil_step ++)= dint;
		}
			else { efbox_prefix= 0, efbox_shift= 2 * j; }
	}
}

void	ei_box(sampI *irow, long *idispptr)
{
	shurt i, ii, ij;
	int da, db, dc, dd;
	long
		sma, smr, smg, smb;
	sampI
		*step_if,
		*subs_if;

	if (efbox_prefix)
	{
			/** non power of two summation */
		for (i= ef_scanx; (i--); irow+= ef_filtsub, idispptr++)
		{
			sma= smr= smg= smb= 0;
			for (ii= ef_filtsub, step_if= irow; (ii--); step_if+= ef_scanwidth)
				for (ij= ef_filtsub, subs_if= step_if; (ij--); subs_if++)
				{
					smr += ebase_xkernal[subs_if-> csr],
					smg += ebase_xkernal[subs_if-> csg],
					smb += ebase_xkernal[subs_if-> csb],
					sma += ebase_xkernal[subs_if-> csa];
				}

			CapDw(smr) CapDw(smg) CapDw(smb) CapDw(sma)
			da= smr >> 7, db= smg >> 7, dc= smb >> 7, dd= sma >> 7;
			*idispptr= (dd << 24) | (dc << 16) | (db << 8) | (da);
		}
	}
	else
	{
			/** power of two summation */
		for (i= ef_scanx; (i--); irow+= ef_filtsub, idispptr++)
		{
			sma= smr= smg= smb= 0;
			for (ii= ef_filtsub, step_if= irow; (ii--); step_if+= ef_scanwidth)
				for (ij= ef_filtsub, subs_if= step_if; (ij--); subs_if++)
				{
					smr += subs_if-> csr,
					smg += subs_if-> csg,
					smb += subs_if-> csb,
					sma += subs_if-> csa;
				}

			da= smr >> efbox_shift, db= smg >> efbox_shift,
			dc= smb >> efbox_shift, dd= sma >> efbox_shift;
			*idispptr= (dd << 24) | (dc << 16) | (db << 8) | (da);
		}
	}
}

static realU		fgauss(realU x)
{
	return exp(- x*x);
	}

static realU 	flancos(realU x)
{
	realU
		dtmp, dva, dvb, afrad;
 
	afrad= ef_filtwin;
	if (x < FILT_TOLERANCE) return 1.0;
	if (x > afrad) return 0.0;

	dtmp= 0.77 * M_PI * x;
	dva= sin(dtmp) / dtmp;
	dvb= sin(x * M_PI / afrad) / (x * M_PI / afrad);
	dtmp= dva * dvb;
	return dtmp;
	}

void	ei_genfilter(FilterType, ScanX,ScanY, FilterSize, Subsamples, Coeffs, fctable)
		int FilterType, ScanX,ScanY, FilterSize, Subsamples;
		short *Coeffs, **fctable;
{
	unsigned short istep, jstep, jcount;
	short *efil_step, *efil_ystep;
	double
		dtmp, dscal, dval, dint,
		sumf, vx, vx0, dx;
	long l;

	/**** quietly fix 0 subsamples per pixel booboo */
	if (Subsamples == 0) Subsamples = 1;

	attr_filttype= FilterType;
	ef_scanx= ScanX, ef_scany= ScanY;
	ef_filtwin= FilterSize, ef_filtsub= Subsamples;
	ef_cpan= 2 * ef_filtwin +1, ef_span= ef_cpan*ef_filtsub;
	ef_radius= ef_filtwin * ef_filtsub;
	ef_half= ef_span / 2, ef_odd = ef_filtsub & 1;
	ef_gap= ef_span * ef_filtsub;

	ebase_xkernal= efil_step= Coeffs,
	ebase_ykernal= efil_ystep= Coeffs + (XSPAN * (ef_half +ef_odd));
	ecur_yfiltertable= fctable;
	dx= 1.0 / ef_filtsub, vx0= ef_filtwin -0.5*(dx-1.0);

	if (ef_span < 2) *ebase_xkernal= 1;
	else
		switch (attr_filttype)
		{
			case FILTER_TRI:
				if (ef_odd) sumf = ((ef_span+1) * (ef_span+1)) >>2;
					else l= ef_span>>1, sumf = l*(l+1);
				dtmp= 128.0 / sumf;
				
				for (istep= ef_half +ef_odd, jcount= 1; (istep --); jcount ++)
				{
					dval= jcount * dtmp;
					*(efil_step ++)= *(efil_ystep ++)= 0;
					for (jstep= XSPAN -1, dint= dval; (jstep --); dint += dval) *(efil_step ++)= dint;
					for (jstep= YSPAN -1, dval /= 16.0, dint= dval; (jstep --); dint += dval) *(efil_ystep ++)= dint;
				}
				break;

			case FILTER_GAUSS:
				for (istep= ef_half, vx= vx0, sumf= 0.0; (istep --); vx-= dx)
					{ sumf+= 2.0 * fgauss(vx); }
				if (ef_odd) sumf += 1.0;
				dscal= 128.0/sumf;

				for (istep= ef_half +ef_odd,vx= vx0; (istep --); vx-= dx)
				{
					dval= dscal * fgauss(vx);
					*(efil_step ++)= *(efil_ystep ++)= 0;
					for (jstep= XSPAN -1, dint= dval; (jstep --); dint += dval) *(efil_step ++)= dint;
					for (jstep= YSPAN -1, dval /= 16.0, dint= dval; (jstep --); dint += dval) *(efil_ystep ++)= dint;
				}
				break;

			case FILTER_SINC:
				for (istep= ef_half, vx= vx0, sumf= 0.0; (istep --); vx-= dx)
					{ sumf+= 2.0 * flancos(vx); }
				if (ef_odd) sumf += 1.0;
				dscal= 128.0/sumf;

				for (istep= ef_half +ef_odd,vx= vx0; (istep --); vx-= dx)
				{
					dval= dscal * flancos(vx);
					*(efil_step ++)= *(efil_ystep ++)= 0;
					for (jstep= XSPAN -1, dint= dval; (jstep --); dint += dval) *(efil_step ++)= dint;
					for (jstep= YSPAN -1, dval /= 16.0, dint= dval; (jstep --); dint += dval) *(efil_ystep ++)= dint;
				}
				break;
			default:
				for (istep= ef_half* (XSPAN + YSPAN), efil_step= ebase_xkernal; (istep--); efil_step++) *efil_step= 0;
				break;
		}
}

void	ei_init(sampI *BaseRowPtr)
{
	ebase_yset= BaseRowPtr;
}

void	ei_reset()
{
	ef_ycount= 0;
	ecur_yset= ebase_yset;
}

void	ei_filtx(sampI *irow)
{
	shurt
		i, fi, fk, 
		spanhalf, spanback;
	short *ek_step;
	long li,
		smr, smg, smb, sma;
	sampI
		*step_if, *subs_if,
		*fill_yf;

	step_if= irow, fk= 2 * ef_radius;
	spanhalf= ef_half + ef_odd,
	spanback= (ef_odd) ? 2 * XSPAN : XSPAN;

	for (i= ef_filtsub; (i--); ecur_yset++, ef_ycount++)
	{
		for (li= ef_scanx, fill_yf= ecur_yset; (li--); step_if+= ef_filtsub, fill_yf+= ef_span)
		{
			sma= smr= smg= smb= 0;
			for (fi= spanhalf, subs_if= step_if, ek_step= ebase_xkernal; (fi--); subs_if++, ek_step+= XSPAN)
			{
				smr += ek_step[subs_if-> csr], smg += ek_step[subs_if-> csg],
				smb += ek_step[subs_if-> csb], sma += ek_step[subs_if-> csa];
			}
			for (fi= ef_half, ek_step-= spanback; (fi--); subs_if++, ek_step-= XSPAN)
			{
				smr += ek_step[subs_if-> csr], smg += ek_step[subs_if-> csg],
				smb += ek_step[subs_if-> csb], sma += ek_step[subs_if-> csa];
			}

			CapDw(smr) CapDw(smg) CapDw(smb) CapDw(sma)
			fill_yf-> csr= smr >> 3,
			fill_yf-> csg= smg >> 3,
			fill_yf-> csb= smb >> 3,
			fill_yf-> csa= sma >> 3;
		}
		step_if += fk; 
	}

	if (ef_ycount == ef_span) ei_reset();
}

static void	efiltymath(long *pixdest)
{
	int istep, jstep,
		da, db, dc, dd;
	long smr, smg, smb, sma;
	short **tbstep, *tbp;
	sampI *step_if;
	
	for (istep= ef_scanx, step_if= ebase_yset; (istep --); pixdest ++)
	{
		smr= smg= smb= sma= 0;
		for (jstep= ef_span, tbstep= ecur_yfiltertable; (jstep --); step_if ++)
		{
			tbp= *(tbstep ++);
			smr += tbp[step_if-> csr],
			smg += tbp[step_if-> csg],
			smb += tbp[step_if-> csb],
			sma += tbp[step_if-> csa];
		}

		CapDw(smr) CapDw(smg) CapDw(smb) CapDw(sma)
		da= smr >> 7, db= smg >> 7, dc= smb >> 7, dd= sma >> 7;
		*pixdest= (dd << 24) | (dc << 16) | (db << 8) | (da);
	}
}

static void	ecofiltymath(long *pixdest)
{
	int istep, jstep,
		da, db, dc, dd;
	long smr, smg, smb, sma;
	short **tbstep, *tbp;
	sampI *step_if;
	
	for (istep= ef_scanx, step_if= ebase_yset; (istep --); pixdest ++)
	{
		smr= smg= smb= sma= 0;
		for (jstep= ef_span, tbstep= ecur_yfiltertable; (jstep --); step_if ++)
		{
			tbp= *(tbstep ++);  if (!tbp) continue;
			smr += tbp[step_if-> csr],
			smg += tbp[step_if-> csg],
			smb += tbp[step_if-> csb],
			sma += tbp[step_if-> csa];
		}

		CapDw(smr) CapDw(smg) CapDw(smb) CapDw(sma)
		da= smr >> 7, db= smg >> 7, dc= smb >> 7, dd= sma >> 7;
		*pixdest= (dd << 24) | (dc << 16) | (db << 8) | (da);
	}
}

#define	YTW	if (!jstep) { jstep= ef_span, yfill= ecur_yfiltertable; }

void	ei_filty(int yplace, long *idispptr)
{
	int
		istep, jstep, jsub,
		yup;
	short
		**yfill, *ycalc,
		*bysum, *ysum;

	yup= ef_half;
	if (!ef_odd) { yup --; }
	ycalc= ebase_ykernal;

		/** deal with beginning */
	if (yplace < ef_cpan)
	{
		yfill= ecur_yfiltertable, bysum= ebase_ykernal + (YSPAN * (ef_half + ef_odd));
		for (istep= YSPAN, ysum= bysum; (istep --); ysum ++) *ysum= 0;
		for(istep= ef_filtsub * (ef_cpan - yplace) +1; (istep --); )
			{ for (jsub= YSPAN, ysum= bysum; (jsub --); ycalc ++) *(ysum ++) += *ycalc; }
		*(yfill ++)= bysum,

		istep= ef_filtsub * yplace -1, jsub= ef_span -1;
		for (jstep= istep - ef_half; (jstep--); istep --, jsub --, ycalc += YSPAN)
			{ *(yfill ++)= ycalc; }
		if (ef_odd) ycalc -= 2 * YSPAN; else ycalc -= YSPAN;
		for (; (istep --); jsub --, ycalc -= YSPAN)
			{ YTW  *(yfill ++)= ycalc; }
		while (jsub --) { *(yfill ++)= NULL; }
		
		ecofiltymath(idispptr);
		return;
	}

		/** deal with middle */
	yfill= ecur_yfiltertable +ef_ycount, jstep= ef_span -ef_ycount;
	for (istep= ef_half + ef_odd; (istep --); jstep --, ycalc += YSPAN)
		{ YTW  *(yfill ++)= ycalc; }
	if (ef_odd) ycalc -= 2 * YSPAN; else ycalc -= YSPAN;
	for (istep= ef_half; (istep--); jstep --, ycalc -= YSPAN)
		{ YTW  *(yfill ++)= ycalc; }
	efiltymath(idispptr);
}

void	ei_finaly(int yplace, long *idispptr)
{
	int
		istep, jv, jstep, kstep;
	short
		**yfill, *ycalc,
		*bysum, *ysum;

	yfill= ecur_yfiltertable +ef_ycount, jstep= ef_span -ef_ycount;
	for (istep= ef_filtsub * (ef_filtwin - yplace); (istep --); jstep --) { YTW  *(yfill ++)= NULL; }
	for (istep= ef_half + ef_odd, ycalc= ebase_ykernal; (istep --); jstep --, ycalc += YSPAN)
		{ YTW  *(yfill ++)= ycalc; }
	if (ef_odd) ycalc -= 2 * YSPAN; else ycalc -= YSPAN;
	istep= ef_half + ef_filtsub * (yplace - ef_filtwin) -1;
	for (jv= ef_half; (istep--); jv --, jstep --, ycalc -= YSPAN)
		{ YTW  *(yfill ++)= ycalc; }

	bysum= ebase_ykernal +  (YSPAN * (ef_half + ef_odd));
	for (istep= YSPAN, ysum= bysum; (istep --); ysum ++) *ysum= 0;
	for (istep= jv; (istep --); ycalc -= 2 * YSPAN)
		{ for (kstep= YSPAN, ysum= bysum; (kstep --); ycalc ++) *(ysum ++) += *ycalc; }
	YTW *(yfill ++)= bysum;

	ecofiltymath(idispptr);
}
