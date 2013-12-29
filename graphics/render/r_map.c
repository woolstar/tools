/** r_map.c **/
#include "rrun.h"

extern shurt	etr_log;

extern long	mem_phase3;
extern er_smap	*magic_im;

static char ixpress[SLINE];

#define TEX_TRUNC	1
#define	TEX_SCALE	1.0/255.0

void tex_lookup(u,v,map,r,g,b, tash)
realU u,v,*r,*g,*b;
er_smap *map;
lung *tash;
{
  long
  	iu, iv,
  	lpixmash;
  byte *p, ipixval;

#ifndef TEX_TRUNC
realU ru,rv,ua,va,au,av;
im_pixel c0,c1,c2,c3,ui,li;
#endif

	if(u < 0.0 || u >= 1.0 || v < 0.0 || v >= 1.0 || map->vdat == NULL)
	{
		*r= *g= *b= 1.0;
		return;	
	 	}

#ifdef TEX_TRUNC
	iu= u * map-> wide;
	iv= v * map-> tall;
	switch(map-> bits)	 /** truncate lookup coords **/
	{
		case 1:
			p= map->vdat+  (long) iv*map->wideplus+iu;
			ipixval= *p, lpixmash= ipixval;
			*r= *g= *b= ipixval *TEX_SCALE;
			break;
		case 2:
			p= map->vdat+ (long) 2*(iv*map->wide+iu);
			ipixval= *(p++), lpixmash= ipixval << 8, *r= ipixval * TEX_SCALE;
			ipixval= *p, lpixmash |= ipixval, *g= ipixval * TEX_SCALE;
			break;
		case 3:
		case 4:
			p= map-> vdat+ (long) 3*(iv*map->wideplus+iu);
			ipixval= *(p++), lpixmash= ipixval << 16, *r= ipixval * TEX_SCALE;
			ipixval= *(p++), lpixmash= ipixval << 8, *g= ipixval * TEX_SCALE;
			ipixval= *p, lpixmash |= ipixval, *b= ipixval * TEX_SCALE;
			break;
		}
	*tash= lpixmash;

#else
switch(map->bits) {  /** bilinearly interpolate **/
case 1:  /** 8 bit;  set all three components **/
ru = u*map->wide;
iu = ru;
ua = ru-iu;
au = 1.0-ua;
rv = v*map->tall;
iv = rv;
va = rv-iv;
av = 1.0-va;
p = map->vdat+iv*map->wideplus+iu;
c0.r = *(p++);
c1.r = *p;
p += map->wide;
c2.r = *(p++);
c3.r = *p;

ui.r = au*c0.r+ua*c1.r;
li.r = au*c2.r+ua*c3.r;
*r = *g = *b = (av*ui.r+va*li.r)*TEX_SCALE;
break;
case 2:  /** 16 bit;  set r = du, g = dv **/
/** truncating; bilin doesn't make sense **/
iu = u*map->wide;
iv = v*map->tall;
p = map->vdat+2*(iv*map->wide+iu);
*r = *(p++);
*g = *p;
break;
case 3:  /** 24 bit **/
case 4:
ru = u*map->wide;
iu = ru;
ua = ru-iu;
au = 1.0-ua;
rv = v*map->tall;
iv = rv;
va = rv-iv;
av = 1.0-va;
p = map->vdat+3*(iv*map->wideplus+iu);
c0.r = *(p++);
c0.g = *(p++);
c0.b = *(p++);
c1.r = *(p++);
c1.g = *(p++);
c1.b = *(p++);
p += 3*(map->wide-1);
c2.r = *(p++);
c2.g = *(p++);
c2.b = *(p++);
c3.r = *(p++);
c3.g = *(p++);
c3.b = *p;

ui.r = au*c0.r+ua*c1.r;
ui.g = au*c0.g+ua*c1.g;
ui.b = au*c0.b+ua*c1.b;

li.r = au*c2.r+ua*c3.r;
li.g = au*c2.g+ua*c3.g;
li.b = au*c2.b+ua*c3.b;

*r = (av*ui.r+va*li.r)*TEX_SCALE;
*g = (av*ui.g+va*li.g)*TEX_SCALE;
*b = (av*ui.b+va*li.b)*TEX_SCALE;
break;
 }
#endif

	}

/** and an integer version for bump-map lookups **/
void	tex_int_lookup(u,v, umir, vmir, map,du,dv, tash)
	realU u,v;
	int umir, vmir;
	er_smap *map;
	short *du,*dv;
	lung *tash;
{
	short isample;
	int iu,iv;
	byte *p, *psection;

	if ((u < 0.0) || (u >= 1.0) || (v < 0.0) || (v >= 1.0) || (map-> vdat == NULL))
		{ *du= *dv= 0;  return; }

	iu= u *map-> wide, iv= v *map-> tall;
	*tash= (iu << 16) + (iv << 3);
	p= map-> vdat +(((long) iv * map-> wide +iu) << 1);
	if (umir)
	{
		if (!iu) { psection= p + (2 *map->wide); } else psection= p;
		psection-= 2;
		isample= (*psection << 8);
		p++;
		}
		else { isample= (*p++) << 8; }
		
	*du= isample >> 8;
	
	if (vmir)
	{
		iv++;
		if (iv == map->tall) { psection= map-> vdat +(iu << 1) +1; }
			else { psection= p + (map-> wide << 1); }
		isample= (*psection << 8);
		}
		else { isample= (*p << 8); }
	*dv= isample >> 8;
	}

void	texv_lookup(double u, double v, ev_smap *istruct, unsigned char *imap, double *imapv, unsigned long *tash)
{
	unsigned char *p;
	int iu, iv;
	
	if ((u < 0.0) || (u >= 1.0) || (v < 0.0) || (v >= 1.0) || (NULL == imap))
		{ *imapv= 0;  return; }

	iu= u * istruct-> imw,
	iv= v * istruct-> imh;
	if (tash) { *tash= ((0xffe & iu) << 12) + ((0xffe & iu) << 3) + ((long) imap << (0x7 & (iu ^ iv))); }
	p= imap +((long) iv * istruct-> imwp +iu);
	*imapv= TEX_SCALE * *p;
	}
	
void	texuv_lookup(double u, double v, ev_smap *istruct, unsigned char *imap, double *imapu, double *imapv, unsigned long *tash)
{
	unsigned char *p;
	short isign;
	int iu, iv;
	
	if ((u < 0.0) || (u >= 1.0) || (v < 0.0) || (v >= 1.0) || (NULL == imap))
		{ *imapu= *imapv= 0;  return; }

	iu= u * istruct-> imw,
	iv= v * istruct-> imh;
	if (tash) { *tash= ((0xffe & iu) << 12) + ((0xffe & iu) << 3) + ((long) imap << (0x7 & (iu ^ iv))); }
	p= imap +(((long) iv * istruct-> imw +iu) << 1);
	isign= *(p ++) << 8, *imapu= TEX_SCALE * (isign >> 8);
	isign= *p << 8, *imapv= TEX_SCALE * (isign >> 8);
	}

void	texc_lookup(double u, double v, ev_smap *istruct, unsigned char *imap, double *cr, double *cg, double *cb, unsigned long *tash)
{
	unsigned char *p;
	int iu, iv;
	
	if ((u < 0.0) || (u >= 1.0) || (v < 0.0) || (v >= 1.0) || (NULL == imap))
		{ *cr= *cg= *cb= 0;  return; }

	iu= u * istruct-> imw,
	iv= v * istruct-> imh;
	if (tash) { *tash= ((0xffe & iu) << 12) + ((0xffe & iu) << 3) + ((long) imap << (0x7 & (iu ^ iv))); }
	p= imap + 3 * ((long) iv * istruct-> imwp +iu);
	*cr= TEX_SCALE * *(p ++),
	*cg= TEX_SCALE * *(p ++),
	*cb= TEX_SCALE * *p;
	}
