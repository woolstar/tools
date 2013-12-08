#include "basic.h"
#include "ebf.h"

FILE	*fsrc, *fdst;

static	int	loadblock(int sz, char *ptr)
{
	int iret;
	iret= fread(ptr, 1, sz, fsrc);
	if (iret < 0) return iret;
	return (iret != sz);
}

static void	x4(int sz, void *ptr)
{
	int istep;
	char *ba, *bb, v;
	for (istep= sz / 4, ba= ptr; (istep --); ba += 3)
	{
		bb= ba +3, v= *ba, *ba= *bb, ba ++, *bb= v;
		bb= ba +1, v= *ba, *ba= *bb, *bb= v;
	}
}
static void x2(int sz, void *ptr)
{
	int istep;
	char *ba, *bb, v;
	for (istep= sz / 2, ba= ptr; (istep --); ba += 2)
		{ bb= ba +1, v= *ba, *ba= *bb, *bb= v; }
}

static void	xS(int sz, void *aptr)
{
	int istep;
	bf_surface *rec;
	char * ptr= aptr ;

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

int		load_embf(FILE *fin, bf_container *resu)
{
	int iret, iorder;
	long l[2], lsz, lm;
	char *ptr;

	iorder= 0;
	fsrc= fin;
	while (2 == (iret= fread((char *) l, sizeof(long), 2, fsrc)))
	{
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
				if (! l[1]) continue;
				BE(3 * sizeof(float)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading VERT block.\n");  RE }
				resu-> vcount= lm, resu-> vptr= ptr;
				break;
			case 'trev':
			case 'TREV':
				if (! l[1]) continue;
				x4(4, (char *) (l +1));
				BE(3 * sizeof(float)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading VERT block.\n");  RE }
				x4(lsz, ptr);
				resu-> vcount= lm, resu-> vptr= ptr;
				break;
			case 'norm':
			case 'NORM':
				if (! l[1]) continue;
				BE(3 * sizeof(float)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading NORM block.\n");  RE }
				resu-> ncount= lm, resu-> nptr= ptr;
				break;
			case 'mron':
			case 'MRON':
				if (! l[1]) continue;
				x4(4, (char *) (l +1));
				BE(3 * sizeof(float)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading NORM block.\n");  RE }
				x4(lsz, ptr);
				resu-> ncount= lm, resu-> nptr= ptr;
				break;
			case 'utex':
			case 'UTEX':
				if (! l[1]) continue;
				BE(2 * sizeof(float)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading VERT block.\n");  RE }
				resu-> ucount= lm, resu-> uptr= ptr;
				break;
			case 'xetu':
			case 'XETU':
				if (! l[1]) continue;
				x4(4, (char *) (l +1));
				BE(2 * sizeof(float)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading VERT block.\n");  RE }
				x4(lsz, ptr);
				resu-> ucount= lm, resu-> uptr= ptr;
				break;
			case 'surf':
			case 'SURF':
				if (! l[1]) continue;
				BE(sizeof(bf_surface)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading VERT block.\n");  RE }
				resu-> scount= lm, resu-> sptr= ptr;
				break;
			case 'frus':
			case 'FRUS':
				if (! l[1]) continue;
				x4(4, (char *) (l +1));
				BE(sizeof(bf_surface)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading VERT block.\n");  RE }
				xS(lsz, ptr);
				resu-> scount= lm, resu-> sptr= ptr;
				break;
			case 'face':
			case 'FACE':
				if (! l[1]) continue;
				BE(sizeof(unsigned short)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading FACE block.\n");  RE }
				resu-> fcount= lm, resu-> fptr= ptr;
				break;
			case 'ecaf':
			case 'ECAF':
				if (! l[1]) continue;
				x4(4, (char *) (l +1));
				BE(sizeof(unsigned short)) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading FACE block.\n");  RE }
				resu-> fcount= lm, resu-> fptr= ptr;
				x2(lsz, ptr);
				break;
			case 'emit':
			case 'EMIT':
				x4(4, (char *) (l +1));
			case 'time':
			case 'TIME':
				BE(1) ptr= malloc(l[1]);  ME
				if (loadblock(lsz, ptr))
					{ printf(" >> failed loading TIME block.\n");  RE }
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
				fseek(fsrc, l[1], 1);
				break;
		}
	}

enod_point:
	return iret;
}

#ifdef DOS
#define	SABLX(xNA, xSIZ, xPTR, xFU)	{ l[0]= xNA, l[1]= xSIZ;  x4(sizeof(l), l);  xFU(xSIZ, xPTR); \
	iret= fwrite((char *) l, sizeof(l), 1, fdst); if (1 != iret) { fclose(fdst);  RE } \
	iret= fwrite(xPTR, sizeof(char), xSIZ, fdst);  if (xSIZ != iret) { fclose(fdst);  RE } }
#define	SABL(xNA, xSIZ, xPTR)	{ l[0]= xNA, l[1]= xSIZ;  x4(sizeof(l), l); \
	iret= fwrite((char *) l, sizeof(l), 1, fdst); if (1 != iret) { fclose(fdst);  RE } \
	iret= fwrite(xPTR, sizeof(char), xSIZ, fdst);  if (xSIZ != iret) { fclose(fdst);  RE } }

#else
#define	SABLX(xNA, xSIZ, xPTR, xFU)	{ l[0]= xNA, l[1]= xSIZ;  \
	iret= fwrite((char *) l, sizeof(l), 1, fdst); if (1 != iret) { fclose(fdst);  RE } \
	iret= fwrite(xPTR, sizeof(char), xSIZ, fdst);  if (xSIZ != iret) { fclose(fdst);  RE } }
#define	SABL(xNA, xSIZ, xPTR)	{ l[0]= xNA, l[1]= xSIZ;  \
	iret= fwrite((char *) l, sizeof(l), 1, fdst); if (1 != iret) { fclose(fdst);  RE } \
	iret= fwrite(xPTR, sizeof(char), xSIZ, fdst);  if (xSIZ != iret) { fclose(fdst);  RE } }

#endif

#define	lV3	(3 * sizeof(float))
#define	lV2	(2 * sizeof(float))
#define	lS1	sizeof(bf_surface)
#define	lF1	sizeof(unsigned short)

int		store_embf(FILE *fout, bf_container *ptr)
{
	int iret;
	long l[2];

	fdst= fout;
	if (NULL == fdst) RE

	l[0]= 'ALGN',
	l[1]= 0;
#ifdef DOS
	x4(sizeof(l), l);
#endif
	fwrite((char *) l, sizeof(l), 1, fdst);

	if (ptr-> vcount && ptr-> vptr)	SABLX('VERT', lV3 * ptr-> vcount, ptr-> vptr, x4)
	if (ptr-> ncount && ptr-> nptr)	SABLX('NORM', lV3 * ptr-> ncount, ptr-> nptr, x4)
	if (ptr-> ucount && ptr-> uptr)	SABLX('UTEX', lV2 * ptr-> ucount, ptr-> uptr, x4)
	if (ptr-> scount && ptr-> sptr)	SABLX('SURF', lS1 * ptr-> scount, ptr-> sptr, xS)
	if (ptr-> fcount && ptr-> fptr)	SABLX('FACE', lF1 * ptr-> fcount, ptr-> fptr, x2)
	if (ptr-> timesize && ptr-> tptr)
		SABL('time', ptr-> timesize, ptr-> tptr)

	return 0;
}

void	clear_embf(bf_container *ptr)
{
	if (ptr-> vcount) { free(ptr-> vptr);  ptr-> vcount= 0, ptr-> vptr= NULL; }
	if (ptr-> ncount) { free(ptr-> nptr);  ptr-> ncount= 0, ptr-> nptr= NULL; }
	if (ptr-> ucount) { free(ptr-> uptr);  ptr-> ucount= 0, ptr-> uptr= NULL; }
	if (ptr-> scount) { free(ptr-> sptr);  ptr-> scount= 0, ptr-> sptr= NULL; }
	if (ptr-> fcount) { free(ptr-> fptr);  ptr-> fcount= 0, ptr-> fptr= NULL; }
}
