#include "basic.h"
#include "ebf.h"
// #include "escene.h"

	// fract
extern int	mem_fract(int n1, int n2, int n3) ;
extern int	fract(void) ;

extern unsigned int	*f_space, *f_final,
					eft_sizespace;
extern bf_flat  	*x_initial;

extern void	em_facearea();

void	embf_facearea(int fn, us *si, vector *vp, vector *nf)
{
	shurt i,
		*s_step;
	vector
		summ,
		vpast, vcur,
		*vap;
	
	s_step= si +fn -1, vap= vp + *s_step;  MpP_cpy(vcur, vap)
	M_zero(summ)
	for (i= fn, s_step= si; (i--); s_step++)
	{
		M_cpy(vpast, vcur)
		vap= vp + *s_step;  MpP_cpy(vcur, vap)
		summ.x+= vpast.y * vcur.z - vcur.y * vpast.z;
		summ.y+= vpast.z * vcur.x - vcur.z * vpast.x;
		summ.z+= vpast.x * vcur.y - vcur.x * vpast.y;
	}
	MPp_cpy(nf, summ)
}

int	epbf_sizeface(us *list, int n, vector *set)
{
	us *fp;
	unsigned int
		i, j,
		*nfp, *nfop;
	long lsz;
	svector ol;
	vector nm, *vlp;
	double dtmp;
	bf_flat *stepc;

	if (NULL == x_initial) { if (mem_fract(4096, 16384, 8192)) return 0; }
	embf_facearea((int) n, list, set, &nm);
	dtmp= M_selfdot(nm);
	if (dtmp < 1e-16) return 0;

	M_cpy(ol, nm)
	if (ol.x < 0.0) ol.x= -ol.x;  if (ol.y < 0.0) ol.y= -ol.y;  if (ol.z < 0.0) ol.z= -ol.z;
	j= (ol.x > ol.y) ? ((ol.x > ol.z) ? 1 : 3) : ((ol.y > ol.z) ? 2 : 3);
	switch (j)
	{
		case 1:  if (nm.x < 0.0) j+= 3;  break;
		case 2:  if (nm.y < 0.0) j+= 3;  break;
		case 3:  if (nm.z < 0.0) j+= 3;  break;
	}

	switch (j)
	{
		case 1:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> y, stepc-> y= vlp-> z; }
			break;
		case 2:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> z, stepc-> y= vlp-> x; }
			break;
		case 3:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> x, stepc-> y= vlp-> y; }
			break;
		case 4:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> z, stepc-> y= vlp-> y; }
			break;
		case 5:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> x, stepc-> y= vlp-> z; }
			break;
		case 6:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> y, stepc-> y= vlp-> x; }
			break;
	}

	/**
	/// load first stack
	/// try fract
	/// try reversed fract (if failed)
	/// output sucessful faces
	*/

#if 1
	for (i= n, j= 1, nfp= f_space +eft_sizespace -1; (i--); j++, nfp--)
		{ *nfp= j; }  *nfp= 0;
	f_space[0]= eft_sizespace -1;
	
	if (fract())
	{
		for (i= n, j= n, nfp= f_space +eft_sizespace -1; (i--); j--, nfp--)
			{ *nfp= j; }  *nfp= 0;
		f_space[0]= 0x8000 | (eft_sizespace -1);
		if (fract()) return 0;
	}
#else
	for (i= n, j= 1, nfp= f_final; (i--); j++, nfp++)
		{ *nfp= j; }  *(nfp++)= 0, *nfp= 0;
#endif

	for (nfp= nfop= f_final, lsz= 0; (*nfp); nfp= nfop +1)
	{
		for (nfop= nfp; (*nfop); nfop ++) lsz ++;
		lsz ++;
	}
	return lsz;
}

int	epbf_createface(us *dest, long *total, us *list, int attr, int n, vector *set)
{
	unsigned short
		*fp, *destfill,
		*listn, *listt, *listl ;
	unsigned int
		i, j, fs,
		*nfp, *nfop;
	long lsz, lct;
	svector ol;
	vector nm, *vlp;
	double dtmp;
	bf_flat *stepc;

	if (NULL == x_initial) { if (mem_fract(4096, 16384, 8192)) return 0; }
	embf_facearea((int) n, list, set, &nm);
	dtmp= M_selfdot(nm);
	if (dtmp < 1e-16) return 0;

	M_cpy(ol, nm)
	if (ol.x < 0.0) ol.x= -ol.x;  if (ol.y < 0.0) ol.y= -ol.y;  if (ol.z < 0.0) ol.z= -ol.z;
	j= (ol.x > ol.y) ? ((ol.x > ol.z) ? 1 : 3) : ((ol.y > ol.z) ? 2 : 3);
	switch (j)
	{
		case 1:  if (nm.x < 0.0) j+= 3;  break;
		case 2:  if (nm.y < 0.0) j+= 3;  break;
		case 3:  if (nm.z < 0.0) j+= 3;  break;
	}

	switch (j)
	{
		case 1:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> y, stepc-> y= vlp-> z; }
			break;
		case 2:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> z, stepc-> y= vlp-> x; }
			break;
		case 3:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> x, stepc-> y= vlp-> y; }
			break;
		case 4:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> z, stepc-> y= vlp-> y; }
			break;
		case 5:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> x, stepc-> y= vlp-> z; }
			break;
		case 6:
			for (i= n, fp= list, stepc= x_initial; (i--); stepc++)
				{ vlp= set + *(fp++), stepc-> x= vlp-> y, stepc-> y= vlp-> x; }
			break;
	}

	/**
	/// load first stack
	/// try fract
	/// try reversed fract (if failed)
	/// output sucessful faces
	*/

#if 1
	for (i= n, j= 1, nfp= f_space +eft_sizespace -1; (i--); j++, nfp--)
		{ *nfp= j; }  *nfp= 0;
	f_space[0]= eft_sizespace -1;
	
	if (fract())
	{
		for (i= n, j= n, nfp= f_space +eft_sizespace -1; (i--); j--, nfp--)
			{ *nfp= j; }  *nfp= 0;
		f_space[0]= 0x8000 | (eft_sizespace -1);
		if (fract()) return 0;
	}
#else
	for (i= n, j= 1, nfp= f_final; (i--); j++, nfp++)
		{ *nfp= j; }  *(nfp++)= 0, *nfp= 0;
#endif

	fs= EPB_FACE, listl= list +n;
	if (attr & EPB_FACTT) { fs |= EPB_FACTT, listt= listl, listl += n; }
		else listt= list;
	if (attr & EPB_FACTN) { fs |= EPB_FACTN, listn= listl, listl += n; }
		else listn= list;
	list --, listn --, listt --;

	for (nfp= nfop= f_final, lsz= 0, lct= 0; (*nfp); nfp= nfop +1)
	{
		destfill= dest +1;
		for (nfop= nfp, i= 0; (j= *nfop); nfop ++, i ++)
			{ *(destfill ++)= list[j], lsz ++; }
		if (attr & EPB_FACTN)
			{ for (nfop= nfp; (j= *nfop); nfop ++) { *(destfill ++)= listn[j], lsz ++; } }
		if (attr & EPB_FACTT)
		{
			for (nfop= nfp; (j= *nfop); nfop ++)
			{
				*(destfill ++)= listt[j], lsz ++;
				*(destfill ++)= 0, lsz ++;
			}
		}
		*dest= fs | i, dest= destfill;
		lsz ++, lct ++;
	}

	*total += lct;
	return lsz;
}

