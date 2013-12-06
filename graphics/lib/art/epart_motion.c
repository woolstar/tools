#include "basic.h"
#include "tscene.h"
#include "tscenelink.h"
#include "eart.h"
#include "epart_word.h"

extern int		epart_holdc;
extern char		epart_hold[SLINE];
extern unsigned long		eactl_track, eactl_space;

	/*********/

static	int	curtrack;
static unsigned short	*spanlist,
						*spancur;
static float			**reflist,
						**refcur, *datacur;

	/*********/

int	mofield[] = 
{
	0,
	MOTION_POS | MOTION_TAR | MOTION_UP | MOTION_FOV,
	MOTION_COLOR | MOTION_POS,
	MOTION_COLOR | MOTION_POS | MOTION_TAR | MOTION_FOV,
	MOTION_COLOR | MOTION_TAR,
	MOTION_POS | MOTION_SCALE | MOTION_Q
} ;
int	mospan[] =
{
	0,
	10,
	6,
	10,
	6,
	10
} ;

int	ato_motion(int av)
{
	switch (av)
	{
		case _MOTION_FOV: return (MOTION_FOV);
		case _COLOR: return (MOTION_COLOR);
		case _SCALE: return (MOTION_SCALE);
		case _MOTION_POSITION: return (MOTION_POS);
		case _MOTION_TARGET: return (MOTION_TAR);
		case _MOTION_UP: return (MOTION_TAR);
		case _MOTION_ROTATION: return (MOTION_ROT);
		case _MOTION_Q: return (MOTION_Q);
	}
	return 0;
}

static int	mo_header(void)
{
	int len;
	long i, av;

	len= 0;
	while (SCAN)
	{
		if (3 != i) break;
		switch (av)
		{
			case _MOTION_FOV:
				len += 1;
				break;
			case _COLOR:
			case _SCALE:
			case _MOTION_POSITION:
			case _MOTION_ROTATION:
			case _MOTION_TARGET:
			case _MOTION_UP:
				len += 3;
				break;
			case _MOTION_Q:
				len += 4;
				break;
			default:
				return len;
		}

		hcSCAN;
		if ('+' != *epart_hold)
			return len;
		
		hcUSE
	}
	return len;
}

#define	CDTBL(ee, xx) { if (lcode & ee) { *(ia ++)= ee, *(ib ++)= len, len += xx; } }
#define SEATBL(ee) { for (ia= ifold, ib= ioff; (*ia && (ee != *ia)); ia ++, ib ++) ;  if (! *ia) RE }

static int	mo_headset(long *la, long *lb, long *lc, int lcode)
{
	int len,
		ifold[16], ioff[16],
		*ia, *ib;
	long i, av;

	ia= ifold, ib= ioff, len= 0;
	CDTBL(MOTION_COLOR, 3)
	CDTBL(MOTION_POS, 3)
	CDTBL(MOTION_TAR, 3)
	CDTBL(MOTION_UP, 3)
	CDTBL(MOTION_SCALE, 3)
	CDTBL(MOTION_Q, 4)
	CDTBL(MOTION_ROT, 4)
	CDTBL(MOTION_FOV, 1)
	*ia= *ib= 0;

	len= 0;
	while (SCAN)
	{
		if (3 != i) break;
		switch (av)
		{
			case _MOTION_FOV:
				SEATBL(MOTION_FOV)
				*(la ++)= *ib, *(lb ++)= 1, *(lc ++)= 0, len ++;
				break;
			case _COLOR:
				SEATBL(MOTION_COLOR)
				*(la ++)= *ib, *(lb ++)= 3, *(lc ++)= 0, len += 3;
				break;
			case _SCALE:
				SEATBL(MOTION_SCALE)
				*(la ++)= *ib, *(lb ++)= 3, *(lc ++)= 0, len += 3;
				break;
			case _MOTION_POSITION:
				SEATBL(MOTION_POS)
				*(la ++)= *ib, *(lb ++)= 3, *(lc ++)= 0, len += 3;
				break;
			case _MOTION_TARGET:
				SEATBL(MOTION_TAR)
				*(la ++)= *ib, *(lb ++)= 3, *(lc ++)= 0, len += 3;
				break;
			case _MOTION_UP:
				SEATBL(MOTION_UP)
				*(la ++)= *ib, *(lb ++)= 3, *(lc ++)= 0, len += 3;
				break;
			case _MOTION_ROTATION:
				SEATBL(MOTION_Q)
				*(la ++)= *ib, *(lb ++)= 4, *(lc ++)= 1, len += 3;
				break;
			case _MOTION_Q:
				SEATBL(MOTION_Q)
				*(la ++)= *ib, *(lb ++)= 4, *(lc ++)= 0, len += 4;
				break;
			default:
				*la= 0, *lb= 0, *lc= 0;
				return len;
		}

		hcSCAN;
		if ('+' != *epart_hold) break;

		hcUSE
	}

	*la= 0, *lb= 0;
	return len;
}

static void	mo_trackset(int span, int n)
{
	while (span --)
	{
		*(spancur ++)= 0,
		*(refcur ++)= datacur, datacur += n;
		curtrack ++;
	}
}

	/*********/

int	epartck_motion(void)
{
	int len, dpth;
	long i, av;

	dpth= 0;
	hcUSE
	len= mo_header();
	if (! len) return 0;

	while (SCAN)
		{ if (('&' != *epart_hold) || (epart_checkval(len))) break;  dpth ++; }

	return dpth;
}

void	epartru_initmotion(us *ispan, float **iref, float *idat)
{
	if (! eactl_track)
	{
		spanlist= spancur= NULL,
		reflist= refcur= NULL,
		datacur= NULL;
		return;
	}

	curtrack= 0;
	bzero(ispan, eactl_track * sizeof(us));
	bzero(iref, eactl_track * sizeof(float *));
	spanlist= spancur= ispan,
	reflist= refcur= iref,
	datacur= idat;
}

static int	mo_line(int tstart, int count, long *lx, long *lw, long *lact)
{
	int fuse, fcmd;
	long i, av;
	unsigned short *sptr;
	float
		ft1, ft2, ft3, **fptr;

	hcUSE
	fuse= 0, fptr= NULL;
	while (count)
	{
		if (! fuse)
		{
			i= tstart + (*lx ++);
			sptr= spanlist +i,
			fptr= reflist +i,
			fuse= *(lw ++),
			fcmd= *(lact ++);
		}
		if (fcmd)
		{
			switch (fcmd)
			{
				case 1:		/** rot-> q */
					if (!( SCAN)) RE  ft1= (float) AHOLDF;  hcUSE  count --;
					if (!( SCAN)) RE  ft2= (float) AHOLDF;  hcUSE  count --;
					if (!( SCAN)) RE  ft3= (float) AHOLDF;  hcUSE  count --;
					if (count < 0) RE
					**fptr= 1.0, (* (fptr ++)) ++, (* (sptr ++)) ++;
					**fptr= 0.0, (* (fptr ++)) ++, (* (sptr ++)) ++;
					**fptr= 0.0, (* (fptr ++)) ++, (* (sptr ++)) ++;
					**fptr= 0.0, (* (fptr ++)) ++, (* (sptr ++)) ++;
					fuse= 0;
				continue;
			}
		}

		if (!( SCAN)) RE
		** fptr= (float) AHOLDF;
		(* (fptr ++)) ++,
		(* (sptr ++)) ++,

		fuse --,
		count --;
		hcUSE
	}
	return 0;
}

#define	CSTRTBL(xx,yy)	if (xx & latt) { if (* str) strcat(str, "+");  strcat(str, yy); }
static void	epm_strhead(int type, char * str)
{
	long latt;
	latt= mofield[type];

	* str= '\0';

	CSTRTBL(MOTION_COLOR, "color")
	CSTRTBL(MOTION_POS, "position")
	CSTRTBL(MOTION_TAR, "target")
	CSTRTBL(MOTION_UP, "up")
	CSTRTBL(MOTION_SCALE, "scale")
	CSTRTBL(MOTION_Q, "q")
	CSTRTBL(MOTION_ROT, "rotation")
	CSTRTBL(MOTION_FOV, "fov")
}

int	epartru_motion(int type, unsigned int *track, int depth)
{
	int
		tlen, ulen;
	long i, av, latt,
		lind[16], lspn[16], lxate[16];

	if (! type || (type >= MO_INVALID)) RE
	latt= mofield[type], tlen= mospan[type];
		if (! latt) RE

	hcUSE
	ulen= mo_headset(lind, lspn, lxate, latt);
	if (ulen < 1) return -1;

	if (! *track)
	{
			/*** assign new tracks & initialize */
		*track= curtrack +1;
		mo_trackset(tlen, depth);
	}

		/*** //\ fill in */
	while (SCAN)
		{ if (!i || ('&' != *epart_hold) || (mo_line(*track -1, ulen, lind, lspn, lxate))) break; }

	return 0;
}

void	epartru_post(int type, int track)
{
	int istep, fuse;
	long lset, lnex;
	unsigned short *sptr;
	float **fptr;

		/** fill in defaults for zero tracks */
	if (! track || ! type || (type >= MO_INVALID)) return;

	sptr= spanlist +track -1, fptr= reflist +track -1;
	for (lset= mofield[type], fuse= 0; (lset); )
	{
		lnex= lset & (~(lset -1));
		switch (lnex)
		{
			case MOTION_POS:
			case MOTION_TAR:
				for (istep= 3; (istep --); sptr ++, fptr ++)
					{ if (! *sptr) { *sptr= 1, **fptr= 0.0, (*fptr) ++; } }
				break;
			case MOTION_COLOR:
			case MOTION_SCALE:
				for (istep= 3; (istep --); sptr ++, fptr ++)
					{ if (! *sptr) { *sptr= 1, **fptr= 1.0, (*fptr) ++; } }
				break;
			case MOTION_UP:
				if (! *sptr)
				{
					*(sptr ++)= 1, **fptr= 0.0, (* (fptr ++)) ++;
					*(sptr ++)= 1, **fptr= 1.0, (* (fptr ++)) ++;
					*(sptr ++)= 1, **fptr= 0.0, (* (fptr ++)) ++;
				}
				else
					{ sptr += 3, fptr += 3; }
				break;
			case MOTION_FOV:
				if (! *sptr) { *sptr= 1, **fptr= 30.0, (*fptr) ++; }
				sptr ++, fptr ++;
				break;
			case MOTION_Q:
				if (! *sptr)
				{
					*(sptr ++)= 1, **fptr= 1.0, (* (fptr ++)) ++;
					for (istep= 3; (istep --); sptr ++, (* (fptr ++)) ++)
						{ *sptr= 1, **fptr= 0.0; }
				}
				else
				{
					sptr ++, fptr ++;
					for (istep= 3; (istep --); sptr ++, fptr ++)
						{ if (! *sptr) { *sptr= 1, **fptr= 0.0, (*fptr) ++; } }
				}
				break;
		}
		lset -= lnex;
	}
}

void	epartru_settle(int n, us *fillc, float **list)
{
	for (; (n --); fillc ++, list ++)
		{ (*list) -= *fillc; }
}

double	epart_motionvalue(int trackoff, tlist_element * ele, int pos, ts_track *ref)
{
	us *dlook;
	float *vlook;

	if (ele)
	{
		if (ele-> tracks) { return (double) ele-> tracks[trackoff]; }
		trackoff += ele-> motion;
	}

	if (trackoff > ref-> count) return 0.0;
	dlook= ref-> depth + (trackoff -1),
	vlook= (ref-> data)[trackoff -1];

	if ((us) pos > *dlook) pos= *dlook;
	return (double) vlook[pos -1];
}

void	epart_motionvec(svector *ptr, int trackoff, tlist_element * ele, int pos, ts_track *ref)
{
	int indnum;
	us *dlook;
	float **vlook, *valptr;

	if (ele)
	{
		if (ele-> tracks)
		{
			valptr= ele-> tracks + trackoff;
			ptr-> x= valptr[0];
			ptr-> y= valptr[1];
			ptr-> z= valptr[2];
			return ;
		}

		trackoff += ele-> motion;
	}

	dlook= ref-> depth + (trackoff -1),
	vlook= ref-> data + (trackoff -1);

	if (trackoff > ref-> count)
		{ ptr-> x= 0.0; }
	else
	{
		valptr= *vlook -1,
		indnum= ((us) pos > *dlook) ? *dlook : pos;
		ptr-> x= valptr[indnum];
		dlook ++, vlook ++, trackoff ++;
	}

	if (trackoff > ref-> count)
		{ ptr-> y= 0.0; }
	else
	{
		valptr= *vlook -1,
		indnum= ((us) pos > *dlook) ? *dlook : pos;
		ptr-> y= valptr[indnum];
		dlook ++, vlook ++, trackoff ++;
	}

	if (trackoff > ref-> count)
		{ ptr-> z= 0.0; }
	else
	{
		valptr= *vlook -1,
		indnum= ((us) pos > *dlook) ? *dlook : pos;
		ptr-> z= valptr[indnum];
	}
}


void	epart_motionarray(float *ptr, int size, int trackoff, tlist_element * ele, int pos, ts_track *ref)
{
	int indnum;
	us *dlook;
	float **vlook, *valptr;

	if (ele)
	{
		if (ele-> tracks)
		{
			valptr= ele-> tracks + trackoff;
			while (size --) { * (ptr ++)= * (valptr ++); }
			return ;
		}

		trackoff += ele-> motion;
	}

	dlook= ref-> depth + (trackoff -1),
	vlook= ref-> data + (trackoff -1);

	while (size && trackoff > ref-> count)
	{
		valptr= *vlook -1,
		indnum= ((us) pos > *dlook) ? *dlook : pos;
		* ptr= valptr[indnum];
		dlook ++, vlook ++, trackoff ++;
		size --, ptr ++ ;
	}

	while (size) { *(ptr ++)= 0.0, size --; }
}

int	epart_setmotionvalue(float value, int offset, tlist_element * ptr, int pos, ts_track *ref)
{
	us *dlook;
	float *vlook;

	if (ptr && ptr-> tracks)
	{
		vlook= ptr-> tracks + offset;
		* vlook= value;
		return 0;
	}
	else 
		if (ptr) offset += ptr-> motion;

	if (offset > ref-> count) RE
	dlook= ref-> depth + (offset -1),
	vlook= (ref-> data)[offset -1];

	if ((us) pos > *dlook) pos= *dlook;
	vlook[pos -1]= value;
	return 0;
}

int	epart_setmotionvec(svector * vptr, int offset, tlist_element * ptr, int pos, ts_track *ref)
{
	us *dlook;
	float *vlook, ** varray;

	if (ptr && ptr-> tracks)
	{
		vlook= ptr-> tracks + offset;
		memcpy(vlook, vptr, sizeof(svector));
		return 0;
	}
	else 
		if (ptr) offset += ptr-> motion;

	if (offset > ref-> count) RE ;
	dlook= ref-> depth + (offset -1),
	varray= ref-> data + (offset -1);

	/* x */
	if ((us) pos <= * dlook) { vlook= (* varray) +pos -1; }
		else { vlook= (* varray) + (*dlook -1); }
	* vlook= vptr-> x;
	dlook ++, varray ++;
	if ((us) pos <= * dlook) { vlook= (* varray) +pos -1; }
		else { vlook= (* varray) + (*dlook -1); }
	* vlook= vptr-> y;
	dlook ++, varray ++;
	if ((us) pos <= * dlook) { vlook= (* varray) +pos -1; }
		else { vlook= (* varray) + (*dlook -1); }
	* vlook= vptr-> z;

	return 0;
}

int	epartmo_print(FILE * fwrite, int type, int offset, tlist_element * ptr, ts_track * tkref)
{
	char str[128];
	unsigned short *depthcheck, maxd;
	unsigned int istep, jstep, ip, t, dt;
	float * fstep, ** ftable;

	if ((type < _E_CAM) || (type > _E_OBJECT))
		return 1;

	epm_strhead(type, str);
	fprintf(fwrite, "\tmotion %s\n", str);

	if (ptr && ptr-> tracks && ! ptr-> motion)
	{
		fprintf(fwrite, "\t&");
		for (istep= mospan[type], fstep= ptr-> tracks; (istep --); fstep ++)
			{ fprintf(fwrite, " %g", * fstep); }
		return 0;
	}

	ip= offset;  if (ptr) ip += (ptr-> motion - 1);
	for (istep= mospan[type], depthcheck= tkref-> depth + ip, maxd= 1;
			(istep --); depthcheck ++)
		{ if (* depthcheck > maxd) maxd= * depthcheck; }

	for (jstep= maxd, t= 1; (jstep --); t ++)
	{
		fprintf(fwrite, "\t\t&");
		for (istep= mospan[type], depthcheck= tkref-> depth + ip, ftable= tkref-> data + ip;
			(istep --); depthcheck ++, ftable ++)
		{
			dt= t;  if (dt > * depthcheck) dt= * depthcheck;
			fstep= * ftable + (dt-1);
			fprintf(fwrite, " %g", * fstep);
		}
		fprintf(fwrite, "\n");
	}

	return 0;
}
