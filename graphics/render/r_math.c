#include "rrun.h"

   /****/

static	double	m[4][4], ma[4][4], mt[4][4];
static	svector	change;

   /****/

#define	F_epsilon       1e-12


#define	MM_MINOR(ax,ay,bx,by)	(ma[ax][ay] * ma[bx][by] - ma[bx][ay] * ma[ax][by])

int	em_invert()
{
   shurt
      i, j, k;
   realU su;

   MM_init(m);
#if 0
   m[x][y]= ?
#endif
   m[0][0]= MM_MINOR(1,1,2,2),
   m[0][1]= -MM_MINOR(0,1,2,2),
   m[0][2]= MM_MINOR(0,1,1,2),
   m[1][0]= -MM_MINOR(1,0,2,2),
   m[1][1]= MM_MINOR(0,0,2,2),
   m[1][2]= -MM_MINOR(0,0,1,2),
   m[2][0]= MM_MINOR(1,0,2,1),
   m[2][1]= -MM_MINOR(0,0,2,1),
   m[2][2]= MM_MINOR(0,0,1,1);

   for (i= 3, su= 0.0; i--; ) su+= ma[i][0] * m[0][i];
   if (su == 0.0) { return 1; }
   su= 1.0 / su;

   for (i= 3; i--; ) for (j= 3; j--; ) m[i][j] *= su;

      /** better fixed offset calc hack (r3) */
   m[0][3]= -(m[0][0] * ma[0][3] + m[0][1] * ma[1][3] + m[0][2] * ma[2][3]),
   m[1][3]= -(m[1][0] * ma[0][3] + m[1][1] * ma[1][3] + m[1][2] * ma[2][3]),
   m[2][3]= -(m[2][0] * ma[0][3] + m[2][1] * ma[1][3] + m[2][2] * ma[2][3]);

#if 1
   /*** testing */

   for (i= 4; i--; ) for (j= 4; j--; )
      { for (k= 4, su= 0.0; k--; ) { su+= m[k][j] * ma[i][k]; }  mt[i][j]= su; }

   j= 0;
   su= mt[0][0] -1.0;  if (su < 0.0) su= -su;  
   if (su < 1e-10) { su= mt[1][1] -1.0;  if (su < 0.0) su= -su;  
      if (su < 1e-10) { su= mt[2][2] -1.0;  if (su < 0.0) su= -su;  
	 if (su < 1e-10) { su= mt[0][1];  if (su < 0.0) su= -su;  
	    if (su < 1e-10) { su= mt[0][2];  if (su < 0.0) su= -su;  
	       if (su < 1e-10) { su= mt[1][0];  if (su < 0.0) su= -su;  
		  if (su < 1e-10) { su= mt[1][2];  if (su < 0.0) su= -su;  
		     if (su < 1e-10) { su= mt[2][0];  if (su < 0.0) su= -su;  
			if (su < 1e-10) { su= mt[2][1];  if (su < 0.0) su= -su;  
			   if (su < 1e-10) 
			      { j= 1; } } } } } } } } }

	if (!j)
	{
		printf("testing result Ma[]  (%g %g %g,  %g %g %g,  %g %g %g)\n",
			ma[0][0], ma[0][1], ma[0][2], ma[1][0], ma[1][1], ma[1][2], ma[2][0], ma[2][1], ma[2][2]);
		printf("testing result M[]  (%g %g %g,  %g %g %g,  %g %g %g)\n",
			m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);
		printf(" >> inverting matrix\n");
		printf("result failed  (%g %g %g,  %g %g %g,  %g %g %g)\n",
			mt[0][0], mt[0][1], mt[0][2], mt[1][0], mt[1][1], mt[1][2], mt[2][0], mt[2][1], mt[2][2]);
		return 1;
	}
#endif

   MM_cpy(ma, m)
   return 0;
   }

#if 0
void	em_save( mblock)
	ep_matrix *mblock;
{
   realU *mstep;

   mstep= mblock-> array;
   *(mstep++)= ma[0][0], *(mstep++)= ma[0][1], *(mstep++)= ma[0][2], *(mstep++)= ma[0][3],
   *(mstep++)= ma[1][0], *(mstep++)= ma[1][1], *(mstep++)= ma[1][2], *(mstep++)= ma[1][3],
   *(mstep++)= ma[2][0], *(mstep++)= ma[2][1], *(mstep++)= ma[2][2], *mstep= ma[2][3];
   }
#endif

void	er_save( vblock)
	er_matrix *vblock;
{
   vblock-> mmx.x= ma[0][0], vblock-> mmx.y= ma[0][1], vblock-> mmx.z= ma[0][2],
   vblock-> mmy.x= ma[1][0], vblock-> mmy.y= ma[1][1], vblock-> mmy.z= ma[1][2],
   vblock-> mmz.x= ma[2][0], vblock-> mmz.y= ma[2][1], vblock-> mmz.z= ma[2][2],
   vblock-> mmw.x= ma[0][3], vblock-> mmw.y= ma[1][3], vblock-> mmw.z= ma[2][3];
   }

#define	ermTERM(A,B)	vresl-> A.B= vblock1-> A.x * vblock2-> mmx.B \
			+ vblock1-> A.y * vblock2-> mmy.B + vblock1-> A.z * vblock2-> mmz.B

void	er_mult( vresl, vblock1, vblock2)
	er_matrix *vresl, *vblock1, *vblock2;
{
   ermTERM(mmx, x),  ermTERM(mmx, y),  ermTERM(mmx, z);
   ermTERM(mmy, x),  ermTERM(mmy, y),  ermTERM(mmy, z);
   ermTERM(mmz, x),  ermTERM(mmz, y),  ermTERM(mmz, z);

   vresl-> mmw.x= vblock1-> mmx.x * vblock2-> mmw.x +
      vblock1-> mmx.y * vblock2-> mmw.y + vblock1-> mmx.z * vblock2-> mmw.z + vblock1-> mmw.x;
   vresl-> mmw.y= vblock1-> mmy.x * vblock2-> mmw.x +
      vblock1-> mmy.y * vblock2-> mmw.y + vblock1-> mmy.z * vblock2-> mmw.z + vblock1-> mmw.y;
   vresl-> mmw.z= vblock1-> mmz.x * vblock2-> mmw.x +
      vblock1-> mmz.y * vblock2-> mmw.y + vblock1-> mmz.z * vblock2-> mmw.z + vblock1-> mmw.z;
   }

void	er_invert3( vresl)
	er_matrix *vresl;
{
   ma[0][0]= vresl-> mmx.x, ma[0][1]= vresl-> mmx.y, ma[0][2]= vresl-> mmx.z, ma[0][3]= 0.0,
   ma[1][0]= vresl-> mmy.x, ma[1][1]= vresl-> mmy.y, ma[1][2]= vresl-> mmy.z, ma[1][3]= 0.0,
   ma[2][0]= vresl-> mmz.x, ma[2][1]= vresl-> mmz.y, ma[2][2]= vresl-> mmz.z, ma[2][3]= 0.0;
   ma[3][0]= 0.0, ma[3][1]= 0.0, ma[3][2]= 0.0, ma[3][3]= 0.0;

   if (em_invert()) return;
   vresl-> mmx.x= ma[0][0], vresl-> mmx.y= ma[0][1], vresl-> mmx.z= ma[0][2],
   vresl-> mmy.x= ma[1][0], vresl-> mmy.y= ma[1][1], vresl-> mmy.z= ma[1][2],
   vresl-> mmz.x= ma[2][0], vresl-> mmz.y= ma[2][1], vresl-> mmz.z= ma[2][2];
   }

   /****/

void	ere_trans(long n, vector *VekPtr)
{
	long i;
	vector
		*vlist_step, 
		vwork;

	for (i= n, vlist_step= VekPtr; (i--); vlist_step++)
	{
		vwork.x= vlist_step-> x, vwork.y= vlist_step-> y, vwork.z= vlist_step-> z;
		vlist_step-> x= vwork.x * ma[0][0] + vwork.y * ma[0][1] + vwork.z * ma[0][2] + ma[0][3];
		vlist_step-> y= vwork.x * ma[1][0] + vwork.y * ma[1][1] + vwork.z * ma[1][2] + ma[1][3];
		vlist_step-> z= vwork.x * ma[2][0] + vwork.y * ma[2][1] + vwork.z * ma[2][2] + ma[2][3];
	}
}

void	ere_trannorm(long n, svector *VekPtr)
{
	long i;
	double dtmp;
	svector
		*vlist_step, 
		vwork, vbalance;

	for (i= n, vlist_step= VekPtr; (i--); vlist_step++)
	{
		vwork.x= vlist_step-> x, vwork.y= vlist_step-> y, vwork.z= vlist_step-> z;
		vbalance.x= vwork.x * ma[0][0] + vwork.y * ma[1][0] + vwork.z * ma[2][0];
		vbalance.y= vwork.x * ma[0][1] + vwork.y * ma[1][1] + vwork.z * ma[2][1];
		vbalance.z= vwork.x * ma[0][2] + vwork.y * ma[1][2] + vwork.z * ma[2][2];

		dtmp= sqrt(M_selfdot(vbalance));  if (dtmp) dtmp= 1.0/dtmp;
		vlist_step-> x= dtmp * vbalance.x,
		vlist_step-> y= dtmp * vbalance.y,
		vlist_step-> z= dtmp * vbalance.z;
	}
}

void	er_faceinfo(er_face *f, vector *v)
{
	int istep;
	unsigned short *lstep;
	vector n, *vi;
	double dtmp;

	em_facenorm(f-> bits, f-> indic_sides, v, &n);
	for (istep= f-> bits, lstep= f-> indic_sides, dtmp= 0.0; (istep --); lstep ++)
		{ vi= v + *lstep, dtmp += M_dot(n, (*vi)); }
	
	M_cpy(f-> face_normal, n)
	f-> plate_d= -dtmp / f-> bits;
	if (n.x < 0.0) n.x= -n.x; if (n.y < 0.0) n.y= -n.y; if (n.z < 0.0) n.z= -n.z;
	istep= RACE_PLANEZ;
	if (n.x > n.y) { if (n.x > n.z) istep= RACE_PLANEX; }
		else { if (n.y > n.z) istep= RACE_PLANEY; }

	f-> bits |= istep;
}

