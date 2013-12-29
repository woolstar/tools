#include "basic.h"
#include "escene.h"

/** 
    sum from i=1,n with j=i+1 except j=1 when i=n:
	(1/2) S (Xj Yi - Xi Yj)
   */

#define	FACE_MN		0x1000
#define FACE_MT		0x2000
#define	FACE_M		0x3000

static flatU	flat_space[FLAT_SPACE];

void	e_colorfix(c)
	colU *c;
{
   realU
      frac, lx, ly, lz,
      hu, lum, ge, ige;

   hu= 6 * c-> r, lum= c-> b, ige= 1.0 - (ge= c-> g), frac= hu - ((int) hu);
   lx= lum * ige, ly= lum * (1.0 - ge * frac), lz= lum * (ige + ge * frac);
   switch ((int) hu)
   {
      case 6:
      case 0:  c-> r= lum, c-> g=  lz, c-> b=  lx;  break;
      case 1:  c-> r=  ly, c-> g= lum, c-> b=  lx;  break;
      case 2:  c-> r=  lx, c-> g= lum, c-> b=  lz;  break;
      case 3:  c-> r=  lx, c-> g=  ly, c-> b= lum;  break;
      case 4:  c-> r=  lz, c-> g=  lx, c-> b= lum;  break;
      case 5:  c-> r= lum, c-> g=  lx, c-> b=  ly;  break;
      }
   }

void	r_rotfix(v)
	vekU *v;
{
   realU
      dtmp, oca, osa, ocb, osb, occ, osc,
      d1p, d1q, d1qi,
      d2p, d2q, d3p, d3q;

   dtmp= (M_PI * v-> x) / 180.0, oca= cos(dtmp), osa= sin(dtmp);
   dtmp= (M_PI * v-> y) / 180.0, ocb= cos(dtmp), osb= sin(dtmp);
   dtmp= (M_PI * v-> z) / 180.0, occ= cos(dtmp), osc= sin(dtmp);

   d1p= oca * osb * occ - osa * osc, dtmp= 1.0 - d1p*d1p;
   if (dtmp > 1e-10)
   {
      d1q= sqrt(dtmp);  d1qi= 1.0 / d1q;
      d2p= d1qi * (oca*osb * osc + osa * occ),
      d2q= d1qi * oca * ocb;
      d3p= d1qi * (oca * osc + osa * osb * occ),
      d3q= d1qi * ocb * occ;
      v-> x= 180 * atan2(d2p, d2q) / M_PI,
      v-> y= 180 * atan2(d1p, d1q) / M_PI,
      v-> z= 180 * atan2(d3p, d3q) / M_PI;
      }
   else
   {
      d3p= osc * ocb, d3q= osb;
      v-> x= 0.0, v-> y= 90.0,
      v-> z= 180 * atan2(d3p, d3q) / M_PI;
      }
   }

void	em_facenorm(fn, si, vp, nf)
	int fn;
	shurt *si;
	vekU *vp, *nf;
{
   shurt i,
      *s_step;
   realU dtmp;
   vekU
      summ,
      vpast, vcur;
   vekU
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
   dtmp= sqrt(M_selfdot(summ));
   if (dtmp!= 0.0) { dtmp= 1.0 /dtmp;  MPp_cpy(nf, dtmp * summ) } 
      else { nf-> x= 0.0, nf-> y= 0.0, nf-> z= 0.0; }
   }

void	em_facearea(fn, si, vp, nf)
	int fn;
	shurt *si;
	vekU *vp, *nf;
{
   shurt i,
      *s_step;
   vekU
      summ,
      vpast, vcur;
   vekU
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

/** 
      for uv,
      k1= v2 / u2v1-v2u1
      k2= v1 / u2v1-v2u1
      _u= _xyz1 . k1 + _xyz2 . k2
   */

void	em_facemath(fsz, fattr, vert, side, norm, ubase)
	int fsz, fattr;
	shurt *side;
	vekU *vert, *norm, *ubase;
{
   shurt 
      i, uct, vct,
      *s_step, *u_step, *u_base;
   long
      un, vn, ucx, vc, uf, vf;
   realU
      dtmp, dbot, dbase;
   vekU
      summ, sumu, sumv, vtmp,
      vpast, vcur, vnext,
      *vap;

#if 1
   em_facenorm(fsz, side, vert, norm);
#else
   s_step= side +fsz -1, vap= vert + *s_step;  MpP_cpy(vcur, vap)
   M_zero(summ)
   for (i= fsz, s_step= side; (i--); s_step++)
   {
      M_cpy(vpast, vcur)
      vap= vert + *s_step;  MpP_cpy(vcur, vap)
      summ.x+= vpast.y * vcur.z - vcur.y * vpast.z;
      summ.y+= vpast.z * vcur.x - vcur.z * vpast.x;
      summ.z+= vpast.x * vcur.y - vcur.x * vpast.y;
      }
   dtmp= sqrt(M_selfdot(summ));
   if (dtmp!= 0.0) { dtmp= 1.0 /dtmp;  MPp_cpy(norm, dtmp * summ) } 
      else { norm-> x= 0.0, norm-> y= 0.0, norm-> z= 0.0; }
#endif

   if (!ubase || !(fattr & FACE_MT)) return;
   M_zero(summ)  M_zero(sumu)  M_zero(sumv)
   i= fsz * ((fattr & FACE_MN) ? 2 : 1);
   u_base= side +i;
   u_step= u_base + 2*(fsz-1), ucx= *(u_step++), vc= *(u_step++);
   u_step= u_base, uf= *(u_step++), vf= *(u_step++);
   s_step= side +fsz -1, vap= vert + *s_step;  MpP_cpy(vcur, vap)
   s_step= side, vap= vert + *(s_step++);  MpP_cpy(vnext, vap)

   for (i= fsz, uct= 0, vct= 0; (i--); )
   {
      un= ucx, ucx= uf, vn= vc, vc= vf;
      M_cpy(vpast, vcur)  M_cpy(vcur, vnext)
      if (!i) { u_step= u_base, s_step= side; }
      uf= *(u_step++), vf= *(u_step++);
      vap= vert + *(s_step++),  MpP_cpy(vnext, vap)
      if ((uf!= ucx) && (vf!= vc) && (vc!= vn))
      {
	 dbase= 1.0 * (vf-vc) / (vc-vn), dbot= (uf-ucx) -dbase * (ucx-un);
	 if (dbot != 0.0)
	 {
	    uct++;
	    dtmp= 1.0 / dbot;
	    M_sub(vtmp, vnext, vcur)  M_plus(sumu, dtmp * vtmp)
	    dbase*= -dtmp;
	    M_sub(vtmp, vcur, vpast)  M_plus(sumu, dbase * vtmp)
	    }
	 }
      else
      {
	 if ((uf!= ucx) && (vf == vc))
	 {
	    uct++;
	    dtmp= 1.0 / (uf-ucx);
	    M_sub(vtmp, vnext, vcur)
	    M_plus(sumu, dtmp * vtmp)
	    }
	 }
      if ((vf!= vc) && (uf!= ucx) && (ucx!= un))
      {
	 dbase= 1.0 * (uf-ucx) / (ucx-un), dbot= (vf-vc) -dbase * (vc-vn);
	 if (dbot != 0.0)
	 {
	    vct++;
	    dtmp= 1.0 / dbot;
	    M_sub(vtmp, vnext, vcur)  M_plus(sumv, dtmp * vtmp)
	    dbase*= -dtmp;
	    M_sub(vtmp, vcur, vpast)  M_plus(sumv, dbase * vtmp)
	    }
	 }
      else
      {
	 if ((vf!= vc) && (uf == ucx))
	 {
	    vct++;
	    dtmp= 1.0 / (vf-vc);
	    M_sub(vtmp, vnext, vcur)
	    M_plus(sumv, dtmp * vtmp)
	    }
	 }
      }
   if (uct)
   {
      dtmp= sqrt(M_selfdot(sumu));
      if (dtmp!= 0.0) { dtmp= 1.0 /dtmp;  MPp_cpy(ubase, dtmp * sumu) } 
	 else { ubase-> x= 0.0, ubase-> y= 0.0, ubase-> z= 0.0; }
      return; 
      }
   if (vct)
   {
      MpP_cpy(vtmp, norm)
      M_cross(summ, sumv, vtmp)
      dtmp= sqrt(M_selfdot(summ));
      if (dtmp!= 0.0) { dtmp= 1.0 /dtmp;  MPp_cpy(ubase, dtmp * summ) } 
	 else { ubase-> x= 0.0, ubase-> y= 0.0, ubase-> z= 0.0; }
      return; 
      }
   ubase-> x= 0.0, ubase-> y= 0.0, ubase-> z= 0.0;
   }

/*****
   NOTE  danger of obscure failure.

   If a second hole is cut out on the same vertex as
     the previous hole was cut, then it might cut from the
     wrong side, because the previous vertex is repeated
     twice in the list.

   What we need to do is a complex 2D normal check, to make sure
     the new edge will be 'inside' the edges and not outside.
     So we'll need to check to two edges around pt_f and see
     if which way they are (concave or convex), then calculate
     the normal pointing outside the shape, and then take the
     dot product with the new edge (pt_f -- pt_h), and make sure
     it's positive, or else check the other occurance(s).

   Now do you understand why I haven't written this?

   DW  4/22/90

   I am writing it, three years later

   DW  4/7/93
*/

void	em_facecut(vk, nf, lf, nh, lh, xf, xh)
	vekU *vk;
	int nf, nh;
	shurt *lf, *lh, *xf, *xh;
{
   shurt
      i, j, ii,
      isear, ipole,
      vf, vh,
      *face_step;
   realU
      dtmp, dl, dh,
      drng, dbst,
      dmatx, dmaty, dvx, dvy;
   flatU
      *hole_space,
      *fl_step, *fl_sear,
      cr1, cr2, cr3, cr4;
   vekU 
      nm, nmpole,
      *vjump;

/*
   /// find bounds of the hole
	bounds didn't work.  find the local maxima
	and since 3d maxima would be eight, flatten to 2d (oh well)
	5-16-92
   /// find the closest point to any corners of the hole
   /// match the closest hole point
   /// shift adjust closest

   ////// and finally handle double spot cuts
   /// scan targeted index for duplicates
   /// calculate interior angles for each possibility
   /// pick and use lowest angle one
   /// done
*/

#define EFC_FLT(AAA, BBB)	while (j--) { ii= *(face_step++), vjump= vk +ii, \
				fl_step-> x= vjump-> AAA, fl_step-> y= vjump-> BBB, fl_step++; }

   if ((nf +nh) > FLAT_SPACE) { *xf= 0, *xh= 0;  return; }

   em_facearea(nf, lf, vk, &nm);
   nmpole= nm;
   if (nm.x<0.0) nm.x= -nm.x;  if (nm.y<0.0) nm.y= -nm.y;  if (nm.z<0.0) nm.z= -nm.z;
   i= 2;  if (nm.x>nm.y) { if (nm.x>nm.z) i= 0; } else { if (nm.y>nm.z) i= 1; }
   switch (i)
   {
      case 0:  ipole= (nmpole.x < 0.0); break;
      case 1:  ipole= (nmpole.y < 0.0); break;
      case 2:  ipole= (nmpole.z < 0.0); break;
      }

   fl_step= flat_space, face_step= lf, j= nf;
   switch (i)
      { case 0: EFC_FLT(y,z) break;  case 1: EFC_FLT(z,x) break;  case 2: EFC_FLT(x,y) break; }
   hole_space= fl_step, face_step= lh, j= nh;
   switch (i)
      { case 0: EFC_FLT(y,z) break;  case 1: EFC_FLT(z,x) break;  case 2: EFC_FLT(x,y) break; }

   fl_step= hole_space, 
   cr1= cr2= cr3= cr4= *fl_step, fl_step++;
   for (i= nh-1; (i--); fl_step++)
   {
      dtmp= fl_step-> x;  if (dtmp < cr1.x) { cr1= *fl_step; } else if (dtmp > cr3.x) cr3= *fl_step;
      dtmp= fl_step-> y;  if (dtmp < cr2.y) { cr2= *fl_step; } else if (dtmp > cr4.y) cr4= *fl_step;
      }

#define	EFC_EBST	if (dtmp < dbst) { dbst= dtmp, j= i; }
   for (i= nf, j= 0, dbst= 1e300, fl_step= flat_space; (i); i--, fl_step++)
   {
      dl= fl_step-> x -cr1.x, dh= fl_step-> y -cr1.y, dtmp= dl*dl +dh*dh;  EFC_EBST
      dl= fl_step-> x -cr2.x, dh= fl_step-> y -cr2.y, dtmp= dl*dl +dh*dh;  EFC_EBST
      dl= fl_step-> x -cr3.x, dh= fl_step-> y -cr3.y, dtmp= dl*dl +dh*dh;  EFC_EBST
      dl= fl_step-> x -cr4.x, dh= fl_step-> y -cr4.y, dtmp= dl*dl +dh*dh;  EFC_EBST
      }

   if (!j) { *xf= 0, *xh= 0;  return; }
   vf= nf -j, fl_step= flat_space +vf, cr1= *fl_step;

   for (i= nh, j= 0, dbst= 1e300, fl_step= hole_space; (i); i--, fl_step++)
   {
      dl= fl_step-> x -cr1.x, dh= fl_step-> y -cr1.y, drng= dl*dl +dh*dh;
      if (drng < dbst) { dbst= drng, j= i; }
      }

   if (!j) { *xf= 0, *xh= 0;  return; }
   vh= nh -j, fl_step= hole_space +vh, cr2= *fl_step;

   for (i= vf, j= vf, ii= nf, fl_step= flat_space +i; (ii--); j= i, dbst= drng)
   {
      if (!i) { i= nf, fl_step= flat_space +i; }  i--, fl_step--;
      dl= fl_step-> x -cr2.x, dh= fl_step-> y -cr2.y, drng= dl*dl +dh*dh;
      if (drng > dbst) break;
      }
   if (j == vf)
   {  for (i= vf+1, ii= nf, fl_step= flat_space +i; (ii--); j= i, dbst= drng, i++, fl_step++)
      {
	 if (i == nf) { i= 0, fl_step= flat_space; }
	 dl= fl_step-> x -cr2.x, dh= fl_step-> y -cr2.y, drng= dl*dl +dh*dh;
	 if (drng > dbst) break;
	 }
      }
   vf= j, fl_step= flat_space +vf, cr1= *fl_step;

   for (i= vh, j= vh, fl_step= hole_space +i; (1); j= i, dbst= drng)
   {
      if (!i) { i= nh, fl_step= hole_space +i; }  i--, fl_step--;
      dl= fl_step-> x -cr1.x, dh= fl_step-> y -cr1.y, drng= dl*dl +dh*dh;
      if (drng > dbst) break;
      }
   if (j == vh)
   {  for (i= vh+1, fl_step= hole_space +i; (1); j= i, dbst= drng, i++, fl_step++)
      {
	 if (i == nh) { i= 0, fl_step= hole_space; }
	 dl= fl_step-> x -cr1.x, dh= fl_step-> y -cr1.y, drng= dl*dl +dh*dh;
	 if (drng > dbst) break;
	 }
      }
   vh= j;

   for (i= nf, ii= lf[vf], j= 0, face_step= lf; (i--); face_step++)
      { if (*face_step == ii) j++; }
   if (j > 1)
   {
      cr1= flat_space[vf], cr2= hole_space[vh];
      dmatx= cr2.x -cr1.x, dmaty= cr2.y -cr1.y;
      for (i= 0, j= nf, dl= 10000.0, face_step= lf, fl_step= flat_space; (i < nf); i++, face_step++, fl_step++)
      {
	 if (*face_step != ii) continue;

	 for (isear= i, fl_sear= fl_step; (1); )
	 {
	    if (!isear) isear= nf, fl_sear= flat_space +nf;  isear--, fl_sear--;
	    dtmp= fl_sear-> x -cr1.x;  if (dtmp < 0.0) dtmp= -dtmp; if (dtmp > 1e-10) break;
	    dtmp= fl_sear-> y -cr1.y;  if (dtmp < 0.0) dtmp= -dtmp; if (dtmp > 1e-10) break;
	    if (isear == i) break;
	    }
	 if (isear == i) continue;

	 cr3.x= fl_sear-> x -cr1.x, cr3.y= fl_sear-> y -cr1.y;
	 dvx= cr3.x * dmatx + cr3.y * dmaty,
	 dvy= cr3.y * dmatx - cr3.x * dmaty;
	 dtmp= atan2(dvy, dvx);  if (ipole) dtmp= -dtmp;
	 if (dtmp < 0.0) dtmp+= M_PI+M_PI;  dh= dtmp;

	 for (isear= i, fl_sear= fl_step; (1); )
	 {
	    isear++, fl_sear++;  if (isear == nf) isear= 0, fl_sear= flat_space;
	    dtmp= fl_sear-> x -cr1.x;  if (dtmp < 0.0) dtmp= -dtmp; if (dtmp > 1e-10) break;
	    dtmp= fl_sear-> y -cr1.y;  if (dtmp < 0.0) dtmp= -dtmp; if (dtmp > 1e-10) break;
	    if (isear == i) break;
	    }
	 if (isear == i) continue;

	 cr4.x= fl_sear-> x -cr1.x, cr4.y= fl_sear-> y -cr1.y;
	 dvx= cr4.x * dmatx + cr4.y * dmaty,
	 dvy= cr4.y * dmatx - cr4.x * dmaty;
	 dtmp= -atan2(dvy, dvx);  if (ipole) dtmp= -dtmp;
	 if (dtmp < 0.0) dtmp+= M_PI+M_PI;  dh+= dtmp;

	 if (dh < dl)
	    { dl= dh, j= i; }
	 }
      if (j!= nf) vf= j;
      }

   *xf= vf, *xh= vh;
   }

#if 0
   /// old face cutting routines in ruleface / ep_aob

   realU
      dtmp, dtnp, drau,
      hx, hy;

   em_facearea((int) lk1, list_f, vt, &nm);
   ji= 2;  
   if (nm.x<0.0) nm.x=-nm.x; if (nm.y<0.0) nm.y=-nm.y; if (nm.z<0.0) nm.z=-nm.z;
   if (nm.x > nm.y) { if (nm.x > nm.z) ji= 0; }
      else { if (nm.y > nm.z) ji= 1; }

   j= lk1, lf= list_f, vf= vek_f;
   switch (ji)
   {
      case 0: while (j--) { vj= vt +(*(lf++)), vf-> x= vj-> y, vf-> y= vj-> z, vf++; } break;
      case 1: while (j--) { vj= vt +(*(lf++)), vf-> x= vj-> z, vf-> y= vj-> x, vf++; } break;
      case 2: while (j--) { vj= vt +(*(lf++)), vf-> x= vj-> x, vf-> y= vj-> y, vf++; } break;
      }

   j= lk2, lf= list_h, vf= vek_h;
   switch (ji)
   {
      case 0: while (j--) { vj= vt +(*(lf++)), vf-> x= vj-> y, vf-> y= vj-> z, vf++; } break;
      case 1: while (j--) { vj= vt +(*(lf++)), vf-> x= vj-> z, vf-> y= vj-> x, vf++; } break;
      case 2: while (j--) { vj= vt +(*(lf++)), vf-> x= vj-> x, vf-> y= vj-> y, vf++; } break;
      }
   
   for (j= lk2-1, ji= 1, lsew= 0, vf= vek_h, dtmp= vf-> y, vf++; (j--); ji++, vf++)
      { if (vf-> y < dtmp) { dtmp= vf-> y, lsew= ji; } }
   vf= vek_h +lsew, hx= vf-> x, hy= vf-> y;

   lcros= 0, vf= vek_f, 
   dtnp= (vf-> x -hx), drau= dtnp*dtnp, dtnp= (vf-> y -hy), drau+= dtnp*dtnp;
   for (j= lk1-1, ji= 1, vf++; (j--); ji++, vf++)
   {
      dtnp= (vf-> x -hx), dtmp= dtnp*dtnp, dtnp= (vf-> y -hy), dtmp+= dtnp*dtnp;
      if (dtmp < drau) { drau= dtmp, lcros= ji; }
      }
   vf= vek_f +lcros;
   vt= vek_h +lsew;

   for (j= lsew, ji= j; (1); )
   {
      if (!ji) { ji= lk2, vt= vek_h +lk2; }  ji--, vt--;
      dtnp= (vf->x - vt->x), dtmp= dtnp*dtnp, dtnp= (vf->y - vt->y), dtmp+= dtnp*dtnp;
      if (dtmp > drau) break;
      j= ji, drau= dtmp;
      }
   if (j == lsew)
   {  while (1)
      {
	 ji++, vt++;  if (ji == lk2) { ji= 0, vt= vek_h; }
	 dtnp= (vf->x - vt->x), dtmp= dtnp*dtnp, dtnp= (vf->y - vt->y), dtmp+= dtnp*dtnp;
	 if (dtmp > drau) break;
	 j= ji, drau= dtmp;
	 }
      }
   lsew= j;
#endif

