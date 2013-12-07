#include "basic.h"

#define	Ra	1629
#define Rm	1048576

static lung	ran_seed;
static int	jitter_n, jitter_nn, jitter_befor;
static realU	jit_scal, jit_offscl;

static realU	jit_nois[9261];

static void	rran_initnoise();

void	rran_initgen()
{
   unsigned long
      time(), gt;

   time (& gt);
   ran_seed= gt % Rm;

   rran_initnoise();
   }

void	rran_initjit(n)
	int n;
{
   realU dtmp;
   if (n < 1) n= 1; else if (n > 16) n= 16;

   if (n > 2)
   {
      jitter_n= n, jitter_nn= n*n, jitter_befor= (n/2)* (n+1);
      dtmp= n, jit_scal= 1.0 / dtmp, jit_offscl= (n & 1) ? 0.0 : (0.5 * jit_scal);
      }
   else
   {
      jitter_n= n, jitter_nn= n*n,
      jit_scal= 0.0;
      if (n == 1)
      {
	 jitter_befor= 1,
	 jit_offscl= 0.0;
	 }
      else
      if (n == 2)
      {
	 jitter_befor= 3,
	 jit_offscl= 0.25;
	 }
      }
   }

int	rran_val(iran)
	int iran;
{
   lung bigtmp;
   realU scal;

   ran_seed= (long) (Ra * ran_seed) % Rm;
   scal= 1.0 * ran_seed * iran / Rm, bigtmp= scal;

   return bigtmp;
   }

realU	rran_vval()
{
   realU scal;

   ran_seed= (long) (Ra * ran_seed) % Rm;
   scal= 1.0 * ran_seed / Rm;

   return scal;
   }

void	rran_jitfirst(fptr)
	flatU *fptr;
{
   if (jitter_n == 2) { fptr-> x= 0.25, fptr-> y= 0.25; }
   else
   {
      fptr-> x= jit_scal * rran_vval() +jit_offscl,
      fptr-> y= jit_scal * rran_vval() +jit_offscl;
      }
   }

void	rran_jitfinal(fptr)
	flatU *fptr;
{
   int i, ix, iy;
   realU
      vlefx, vlefy;

   if (jitter_n == 2)
   {
      fptr-> x= -0.25, fptr-> y= -0.25, fptr++;
      fptr-> x= 0.25, fptr-> y= -0.25, fptr++;
      fptr-> x= -0.25, fptr-> y= 0.25;
      return ;
      }
   for (iy= jitter_n, i= jitter_befor, vlefy= 0.5 * (jit_scal-1.0); (iy--); vlefy+= jit_scal)
      for (ix= jitter_n, vlefx= 0.5 * (jit_scal-1.0); (ix--); i--, vlefx+= jit_scal)
      {
	 if (!i) continue;
	 fptr-> x= jit_scal * rran_vval() +vlefx,
	 fptr-> y= jit_scal * rran_vval() +vlefy;
	 fptr++;
	 }
   }

#if 0
int	sng_rand(n)
	ul  n;
{
   int bign;
   dussel
      dfix;

   seex= (ul) (Ra * seex +1) % Rm;
   if (!n) return 0;
   dfix= 2.0 * (seex * n) / Rm, bign= dfix -n;
   return bign;
   }
#endif

static void	rran_initnoise()
{
   realU
      *p, *q, *q0, *r0,
      dtmp;
   shurt
      i, j, k;

   p= jit_nois;
   for (k= 20, r0= p; (k--); )
   {
      for (j= 20, q0= p; (j--); p++)
      {
	 for (i= 20, q= p; (i--); p++) *p= 1e-3 * (rran_val(1000) + 1e-3 * rran_val(1000));
	 *p= *q;
	 }
      for (j= 21; (j--); p++, q0++) *p= *q0;
      }
   for (k= 441; (k--); p++, r0++) *p= *r0;
   }

realU	rran_noisev(vx, vy, vz)
	realU vx, vy, vz;
{
   realU
      dtmp, dfract,
      v1, v2, v3, v4,
      ve0, ve1, ve2,
      vf0, vf1, vf2,
      *p, *p0, *p1, *p2;
   unsigned int i, j, k, ii;

   dfract= vx -floor(vx), dtmp= 20.0 * dfract, i= dtmp, vf0= dtmp -i, ve0= 1.0 -vf0;
   dfract= vy -floor(vy), dtmp= 20.0 * dfract, j= dtmp, vf1= dtmp -j, ve1= 1.0 -vf1;
   dfract= vz -floor(vz), dtmp= 20.0 * dfract, k= dtmp, vf2= dtmp -k, ve2= 1.0 -vf2;

   ii= ((k %20) * 21 + (j % 20)) * 21 + (i % 20);
   p0= p1= p2= jit_nois +ii;

   p= p0+1, v1= ve0 * *p0 + vf0 * *p;
   p1+= 21, p= p1+1, v2= ve0 * *p1 + vf0 * *p;
   p2+= 441, p= p2+1, v3= ve0 * *p2 + vf0 * *p;
   p2+= 21, p= p2+1, v4= ve0 * *p2 + vf0 * *p;

   dtmp= ve2 * (ve1 * v1 + vf1 * v2) + vf2 * (ve1 * v3 + vf1 * v4);
   return dtmp;
   }

realU	rran_noises(vs)
	realU vs;
{
   realU
      dtmp, dfract,
      ve0, vf0,
      *p0, *p1;
   unsigned int i, ii;

   dfract= vs -floor(vs), dtmp= 20.0 * dfract, i= dtmp, vf0= dtmp -i, ve0= 1.0 -vf0;
   ii= i % 20, p0= jit_nois +ii, p1= p0+1, dtmp= ve0 * *p0 + vf0 * *p1;
   return dtmp;
   }
