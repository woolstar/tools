#include "basic.h"
#include "ebf.h"

static void	x4(int sz, char *ptr)
{
	int istep;
	char *ba, *bb, v;
	for (istep= sz / 4, ba= ptr; (istep --); ba += 3)
	{
		bb= ba +3, v= *ba, *ba= *bb, ba ++, *bb= v;
		bb= ba +1, v= *ba, *ba= *bb, *bb= v;
	}
}
static void x2(int sz, char *ptr)
{
	int istep;
	char *ba, *bb, v;
	for (istep= sz / 2, ba= ptr; (istep --); ba += 2)
		{ bb= ba +1, v= *ba, *ba= *bb, *bb= v; }
}

static void	xS(int sz, char *ptr)
{
	int istep;
	bf_surface *rec;

	for (istep= sz / (sizeof(bf_surface)); (istep --); )
	{
		ptr += sizeof(rec-> name);
		x4(3 * sizeof(float), ptr);
		ptr += 3 * sizeof(float);
	}
}

#define	BE(xxx)	if (l[1] < 1) { printf(" >> block size error %d\n", l[1]);  RE } else lsz= l[1]; \
				lm= lsz / (xxx);  if ((xxx * lm) != lsz) \
					{ printf(" >> block reg error %d - %d\n", (xxx * lm), lsz);  RE }
#define ME	if (NULL == ptr) { printf(" >> unable to allocate %s\n", mem_name(lsz));  RE }

#define	mU(xx)	len -= xx, src += xx

int		mem_embf(long len, unsigned char * src, bf_container *resu)
{
	int iret, iorder;
	long l[2], lsz, lm;
	char *ptr;

	iret= 0;
	iorder= 0;
	while (len > sizeof(l))
	{
		memcpy(l, src, sizeof(l));  mU(sizeof(l));

		if (! l[1]) continue;
		switch (* l)
		{
			case 'algn':
			case 'ALGN':
				iorder= 0;
				break;
			case 'ngla':
			case 'NGLA':
				iorder= 1;
				break;

			case 'vert':
			case 'VERT':
				BE(3 * sizeof(float)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				resu-> vcount= lm, resu-> vptr= ptr;
				break;
			case 'trev':
			case 'TREV':
				x4(4, (char *) (l +1));
				BE(3 * sizeof(float)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				x4(lsz, ptr);
				resu-> vcount= lm, resu-> vptr= ptr;
				break;
			case 'norm':
			case 'NORM':
				BE(3 * sizeof(float)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				resu-> ncount= lm, resu-> nptr= ptr;
				break;
			case 'mron':
			case 'MRON':
				x4(4, (char *) (l +1));
				BE(3 * sizeof(float)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				x4(lsz, ptr);
				resu-> ncount= lm, resu-> nptr= ptr;
				break;
			case 'utex':
			case 'UTEX':
				BE(2 * sizeof(float)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				resu-> ucount= lm, resu-> uptr= ptr;
				break;
			case 'xetu':
			case 'XETU':
				x4(4, (char *) (l +1));
				BE(2 * sizeof(float)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				x4(lsz, ptr);
				resu-> ucount= lm, resu-> uptr= ptr;
				break;
			case 'surf':
			case 'SURF':
				BE(sizeof(bf_surface)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				resu-> scount= lm, resu-> sptr= ptr;
				break;
			case 'frus':
			case 'FRUS':
				x4(4, (char *) (l +1));
				BE(sizeof(bf_surface)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				xS(lsz, ptr);
				resu-> scount= lm, resu-> sptr= ptr;
				break;
			case 'face':
			case 'FACE':
				BE(sizeof(unsigned short)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				resu-> fcount= lm, resu-> fptr= ptr;
				break;
			case 'ecaf':
			case 'ECAF':
				x4(4, (char *) (l +1));
				BE(sizeof(unsigned short)) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				resu-> fcount= lm, resu-> fptr= ptr;
				x2(lsz, ptr);
				break;
			case 'emit':
			case 'EMIT':
				x4(4, (char *) (l +1));
			case 'time':
			case 'TIME':
				BE(1) ptr= malloc(l[1]);  ME
				memcpy(ptr, src, lsz);  mU(lsz);
				resu-> timesize= lm, resu-> tptr= ptr;
				break;
			case 'done':
			case 'DONE':
			case 'enod':
			case 'ENOD':
				iret= 0;
				goto enod_point;

			default:
				if (iorder) { x4(4, (char *) (l +1)); }
				mU(l[1]);
				break;
		}
	}

enod_point:
	return iret;
}

