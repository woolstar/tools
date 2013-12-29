#include "rrun.h"
#include "tscene.h"
#include "tscenelink.h"

   /****/

extern int	global_time;
extern double	epart_motionvalue(int, tlist_element *, int, ts_track *);

   /****/

static	double	m[4][4], ma[4][4], mt[4][4];
static	svector	change;

   /****/

#define	F_epsilon       1e-12

static void em_init()
{
   shurt i, j;
   MM_init(ma)  MM_ident(ma);
   }

static void em_mult()
{
   short i, j, k;  realU su;
   for (i= 4; i--; ) for (j= 4; j--; )
      { for (k= 4, su= 0.0; k--; ) { su+= m[k][j] * ma[i][k]; }  mt[i][j]= su; }
   MM_cpy(ma, mt)
   }

static void em_rotx()
{
   shurt i,j;
   realU c, s, rad;

   MM_init(m)  MM_ident(m)
   rad= change.x * M_PI / 180.0, c= cos(rad), s= sin(rad);
   m[1][1]= c, m[2][2]= c;
   m[2][1]= s, m[1][2]= -s;
   em_mult();
   }

static void em_roty()
{
   shurt i,j;
   realU c, s, rad;

   MM_init(m)  MM_ident(m)
   rad= change.y * M_PI / 180.0, c= cos(rad), s= sin(rad);
   m[0][0]= c, m[2][2]= c;
   m[2][0]= -s, m[0][2]= s;
   em_mult();
   }

static void em_rotz()
{
   shurt i,j;
   realU c, s, rad;

   MM_init(m) MM_ident(m)
   rad= change.z * M_PI / 180.0, c= cos(rad), s= sin(rad);
   m[0][0]= c, m[1][1]= c;
   m[1][0]= s, m[0][1]= -s;
   em_mult();
   }

static void	em_rotq(quaternion *q)
{
	unsigned int i, j;
	vector vtmp;

	MM_init(m)

	QM_x(vtmp, (* q));  m[0][0]= vtmp.x, m[0][1]= vtmp.y, m[0][2]= vtmp.z ;
	QM_y(vtmp, (* q));  m[1][0]= vtmp.x, m[1][1]= vtmp.y, m[1][2]= vtmp.z ;
	QM_z(vtmp, (* q));  m[2][0]= vtmp.x, m[2][1]= vtmp.y, m[2][2]= vtmp.z ;
	m[3][3]= 1.0;
	
	em_mult();
}

static void em_scale()
{
   shurt i,j;

   MM_init(m)
   m[0][0]= change.x, m[1][1]= change.y, m[2][2]= change.z, m[3][3]= 1.0;
   em_mult();
   }

static void em_trnla()
{
   shurt i,j;

   MM_init(m)  MM_ident(m)
   m[0][3]= change.x, m[1][3]= change.y, m[2][3]= change.z;
   em_mult();
   }

#define RM_TRAG(eee)	trag_lookup= *trag_step;  if (*trog_step) { trag_lookup += global_time; } \
			eee= (realU) *trag_lookup, trag_step++, trog_step++;

static void	tm_loadmtsr(tlist_element * ref, ts_track * tbase)
{
	svector svtmp;
	quaternion  qv;
	
	epart_motionvec(& change, 0, ref, global_time +1, tbase);
	if ((change.x != 0.0) || (change.y!= 0.0) || (change.z!= 0.0)) em_trnla();

	qv.s= epart_motionvalue(+6, ref, global_time +1, tbase);
		epart_motionvec(& svtmp, +7, ref, global_time +1, tbase);
	qv.i= svtmp.x, qv.j= svtmp.y, qv.k= svtmp.z;
	em_rotq(& qv);

	epart_motionvec(& change, +3, ref, global_time +1, tbase);
	if ((change.x != 1.0) || (change.y!= 1.0) || (change.z!= 1.0)) em_scale();
}

static void	tm_loadmm(tlist_element * ref, ts_track * tbase)
{
	svector svtmp;

	/*** ok order */
	epart_motionvec(& svtmp, 0, ref, global_time +1, tbase);
		m[0][0]= svtmp.x, m[0][1]= svtmp.y, m[0][2]= svtmp.z;
	epart_motionvec(& svtmp, 3, ref, global_time +1, tbase);
		m[1][0]= svtmp.x, m[1][1]= svtmp.y, m[1][2]= svtmp.z;
	epart_motionvec(& svtmp, 6, ref, global_time +1, tbase);
		m[2][0]= svtmp.x, m[2][1]= svtmp.y, m[2][2]= svtmp.z;
	epart_motionvec(& svtmp, 9, ref, global_time +1, tbase);
		m[0][3]= svtmp.x, m[1][3]= svtmp.y, m[2][3]= svtmp.z;

   m[3][0]= 0.0, m[3][1]= 0.0, m[3][2]= 0.0, m[3][3]= 1.0;
   em_mult();
}

	/** create a matrix stack from the element & all parents **/
void	tm_createm(tlist_element *ptr, ts_track *trackptr)
{
	tlist_element *parentstep, *parentlast;

	em_init();
	for (parentlast= ptr-> parent;
		(parentlast && parentlast-> parent);  parentlast= parentlast-> parent) ;
	if (parentlast)
	{
		while (parentlast != ptr)
		{
/*
			if (parentlast-> motion < 0) tm_loadmm(- parentlast-> motion, trackptr);
				else tm_loadmtsr(parentlast-> motion, trackptr);
*/
			tm_loadmtsr(parentlast, trackptr);
			for (parentstep= ptr; (parentstep && (parentstep-> parent!= parentlast));
				parentlast= parentlast-> parent) ;
			parentlast= parentstep;
		}
	}
/*
	if (ptr-> motion < 0) tm_loadmm(- ptr-> motion, trackptr);
		else tm_loadmtsr(ptr-> motion, trackptr);
*/
	tm_loadmtsr(ptr, trackptr);
}
