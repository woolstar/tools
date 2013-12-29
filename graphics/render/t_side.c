#include "rrun.h"
#include "escene.h"

   /***/

static flatU	frame_side[FLAT_SPACE];

   /***/

#define	IX_edge	cre_sides-> m= side_m= side_dy / side_dx, \
		cre_sides-> b= frame_step-> y - side_m * frame_step-> x, \
		cre_sides-> bits= (((side_dx > 0.0) != side_neg) ? RSIDE_ABOVE : RSIDE_BELOW) |

#define IY_edge	cre_sides-> m= side_m= side_dx / side_dy, \
		cre_sides-> b= frame_step-> x - side_m * frame_step-> y, \
		cre_sides-> bits= (((side_dy < 0.0) != side_neg) ? RSIDE_ABOVE : RSIDE_BELOW) |

#define	RB_SIDEX	IX_edge RSIDE_X
#define	RB_SIDEY	IY_edge RSIDE_Y
#define	RB_SIDEIX	IX_edge RSIDE_X | RSIDE_INSIDE | (i+2);
#define	RB_SIDEIY	IY_edge RSIDE_Y | RSIDE_INSIDE | (i+2);

void	rbuild_sides(er_side ** side_el, vector *vbase, er_face *faceptr)
{
	shurt
		i, side_neg,
		*ind_step;
	unsigned int fsize;
	double
		side_dx, side_dy, 
		side_m, 
		adx, ady;
	flatU *frame_step, *frame_fol;
	vector *vek_point;
	er_side *cre_sides;

	#define	RBS_VIP		vek_point= vbase + *ind_step

	fsize= faceptr-> bits & 0x3ff;
	switch (faceptr-> bits & RACE_PLANEM)
	{
		case RACE_PLANEX:
			side_neg= (faceptr-> face_normal.x < 0.0);
			for (i= fsize, ind_step= faceptr-> indic_sides, frame_step= frame_side;
					(i--); ind_step++, frame_step++)
				{ RBS_VIP, frame_step-> x= vek_point-> y, frame_step-> y= vek_point-> z; }
			break;
		case RACE_PLANEY:
			side_neg= (faceptr-> face_normal.y < 0.0);
			for (i= fsize, ind_step= faceptr-> indic_sides, frame_step= frame_side;
					(i--); ind_step++, frame_step++)
				{ RBS_VIP, frame_step-> x= vek_point-> z, frame_step-> y= vek_point-> x; }
			break;
		case RACE_PLANEZ:
			side_neg= (faceptr-> face_normal.z < 0.0);
			for (i= fsize, ind_step= faceptr-> indic_sides, frame_step= frame_side;
					(i--); ind_step++, frame_step++)
				{ RBS_VIP, frame_step-> x= vek_point-> x, frame_step-> y= vek_point-> y; }
			break;
	}

	faceptr-> bound_sidea= cre_sides= *side_el;
	for (i= fsize -1, frame_fol= frame_side, frame_step= frame_side +1;
		(i--); frame_fol= frame_step, frame_step++)
	{
		side_dx= frame_step-> x -frame_fol-> x, adx= (side_dx < 0.0) ? -side_dx : side_dx,
		side_dy= frame_step-> y -frame_fol-> y; ady= (side_dy < 0.0) ? -side_dy : side_dy;
		if ((0.0 == adx) && (0.0 == ady))
		{
			faceptr-> bits= 0;
			return ;
		}

		if (adx > ady) { RB_SIDEX; } else { RB_SIDEY; }
		cre_sides++;

		if (!i)
		{
			side_dx= frame_side-> x -frame_step-> x, adx= (side_dx < 0.0) ? -side_dx : side_dx,
			side_dy= frame_side-> y -frame_step-> y; ady= (side_dy < 0.0) ? -side_dy : side_dy;
			if ((0.0 == adx) && (0.0 == ady))
			{
				faceptr-> bits= 0;
				return ;
			}
			if (adx > ady)
				{ RB_SIDEX | RSIDE_END; } else { RB_SIDEY | RSIDE_END; }
			cre_sides++;
		}
	}
	if (faceptr-> bits & 0xc000)
	{
		for (i= fsize -3, frame_fol= frame_side, frame_step= frame_side +2; (i--); frame_step++)
		{
			side_dx= frame_step-> x -frame_fol-> x, adx= (side_dx < 0.0) ? -side_dx : side_dx,
			side_dy= frame_step-> y -frame_fol-> y; ady= (side_dy < 0.0) ? -side_dy : side_dy;
			if ((0.0 == adx) && (0.0 == ady))
			{
				faceptr-> bits &= ~ 0xc000;
				return ;
			}
			if (adx > ady) { RB_SIDEIX; } else { RB_SIDEIY; }
			cre_sides++;
		}
	}
	*side_el= cre_sides;
}

