#include "rrun.h"

extern ray_element	*current;

#define RZMASK		0x3ff

void calc_spot(er_face *fip, int n)
{
	short i;
	shurt
		j, k,
		*dr, *dp;
	realU
		qx, qy, 
		p0x, p0y, p1x, p1y, p2x, p2y,
		ba,bb,bc, btmp;
	vector
		nm, mix, *v, *vr;
	svector *sv, *vn;

	k= fip-> bits;
	i= (k & RZMASK) -n -1;
	j= fip-> bits & RACE_PLANEM;
	dr= fip-> indic_sides, dp= dr +i;
	vr= fip-> shading-> global_v, v= vr + (*dr);
	switch (j)
	{
		case RACE_PLANEX: 
			qx= current-> hit_p.y, qy= current-> hit_p.z;  
			p0x= v-> y, p0y= v-> z;
			v= vr + (*dp), p1x= v-> y, p1y= v-> z, dp++;
			v= vr + (*dp), p2x= v-> y, p2y= v-> z;
			break;
		case RACE_PLANEY: 
			qx= current-> hit_p.z, qy= current-> hit_p.x;  
			p0x= v-> z, p0y= v-> x;
			v= vr + (*dp), p1x= v-> z, p1y= v-> x, dp++;
			v= vr + (*dp), p2x= v-> z, p2y= v-> x;
			break;
		case RACE_PLANEZ: 
			qx= current-> hit_p.x, qy= current-> hit_p.y;  
			p0x= v-> x, p0y= v-> y;
			v= vr + (*dp), p1x= v-> x, p1y= v-> y, dp++;
			v= vr + (*dp), p2x= v-> x, p2y= v-> y;
			break;
	}
	ba= (p2x -p1x) * (qy -p1y) - (p2y -p1y) * (qx -p1x);
		btmp= (p2x -p1x) * (p0y -p1y) - (p2y -p1y) * (p0x -p1x), ba/= btmp;
	bb= (p0x -p2x) * (qy -p2y) - (p0y -p2y) * (qx -p2x);
		btmp= (p0x -p2x) * (p1y -p2y) - (p0y -p2y) * (p1x -p2x), bb/= btmp;
	bc= (p1x -p0x) * (qy -p0y) - (p1y -p0y) * (qx -p0x);
		btmp= (p1x -p0x) * (p2y -p0y) - (p1y -p0y) * (p2x -p0x), bc/= btmp;

	dr+= (k & RZMASK);
	if ((ba < -0.1) || (ba > 1.1) ||
		(bb < -0.1) || (bb > 1.1) ||
		(bc < -0.1) || (bc > 1.1))
	{
		M_cpy(current-> hit_n, fip-> face_normal)
		return ;
	}
	if (k & RACE_NORMAL)
	{
		dp= dr + i, vn= fip-> shading-> global_n, sv= vn +(*dr);
		MpP_cpy(mix, sv)  M_cpy(nm, ba * mix)
		sv= vn + (*dp),  MpP_cpy(mix, sv)  M_plus(nm, bb * mix)  dp++;
		sv= vn + (*dp),  MpP_cpy(mix, sv)  M_plus(nm, bc * mix)
		btmp= M_selfdot(nm);
		if (btmp > 2.0)
		{
			M_cpy(current-> hit_n, fip-> face_normal)
			return ;
		}
		if (btmp > 1e-12)
		{
			btmp= 1.0 / sqrt(btmp);
			M_scale(nm, btmp);
		}
		btmp= M_dot(nm, fip-> face_normal);
		M_cpy(current-> hit_n, nm)
		dr+= (k & RZMASK);
	}

	if (k & RACE_TEXTURE)
	{
		/*****
			Textures are for users, I'm just a hacker.
				It gets left to the side of the road.
			DW	9/24

			It sure seems kind of meek, just a few mixes of shorts.
				Happily, my soup is not getting cold.
			DW  11/6

			There are now two kinds of textures now, uv points 
				and projections.  Projections override dgs because we say so.
				But they get moved to somewhere else.
			DW  8-4-92
			DW  8-6-92
			*/
		mix.x= 0.0, mix.y= 0.0, dp= dr;
		mix.x= ba * (*(dp++)), mix.y= ba * (*dp);
		dp= dr + (2 * i),
		mix.x+= bb * (*(dp++)), mix.y+= bb * (*(dp++)),
		mix.x+= bc * (*(dp++)), mix.y+= bc * (*dp);
		current-> paint_u= mix.x * 3.0517578125e-05;
		current-> paint_v= mix.y * 3.0517578125e-05;
	}
}

