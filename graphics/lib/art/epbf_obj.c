#include "basic.h"
#include "ebf.h"

static	FILE *fin;
static	char	sline[8192];

static int	rdline(void)
{
	int c, safe, merge;
	char *p;

	if (NULL == fin) RE
	p= sline, safe= sizeof(sline);
	merge= 0;
	while (safe && (EOF != (c= fgetc(fin))))
	{
		if (('\n' == c) || ('\r' == c))
			{ if (! merge && (p != sline)) break;  if (merge) merge --;  continue; }
		if ('\\' == c) { merge ++; }
			else { *(p ++)= c, safe --; }
	}
	if (EOF == c)
	{
		fclose(fin);
		fin= NULL;
	}
	*p= '\0';
	return 0;
}

static int	surfhack(char *p, bf_surface * x)
{
	char c, * q;

	P_end
	strcpy(x-> name, q) ;
	P_word  P_word  P_word  P_word
	x-> r= atof(p) ;  P_word
	x-> g= atof(p) ;  P_word
	x-> b= atof(p) ;
	return 1 ;
}

static int	surfparse(char *p, bf_surface *x, bf_surface *base)
{
	char c, *q;

	P_end

	while ((base != x) && strcmp(base-> name, q)) { base ++; }
	if (base != x) return 0;

	printf(") srf %s\n", q);

	strcpy(x-> name, q);
	x-> r= atof(p);  P_word
	x-> g= atof(p);  P_word
	x-> b= atof(p);
	return 1;
}

static int	surflib(FILE * fsrc, bf_surface * x)
{
	int	ct = 0 ;
	bf_surface * cur = NULL ;
	char c, * p, * q, strtmp[256] ;

	while (fgets(strtmp, sizeof(strtmp), fsrc))
	{
		p= strtmp;  P_space ;
		if (! strncmp(p, "newmtl", 6)) {
			if (cur) ct ++ ;
			cur= x ++ ;
			P_word  P_end
			strcpy(cur->name, q) ;
		}
		else if (! strncmp(p, "Kd", 2)) {
			if (cur) {
				P_word
				cur->r= atof(p) ;  P_word
				cur->g= atof(p) ;  P_word
				cur->b= atof(p) ;
			}
		}
	}

	if (cur) ct ++ ;
	return ct ;
}

static void	vecparse(char *p, svector *v)
{
	char c;

	v-> x= atof(p);  P_word  if (! *p) { v-> y= v-> z= 0.0;  return; }
	v-> y= atof(p);  P_word  if (! *p) { v-> z= 0.0;  return; }
	v-> z= atof(p);
}
static void	vec2parse(char *p, float *v)
{
	char c;

	v[0]= atof(p);  P_word  if (! *p) { v[1]= 0.0;  return; }
	v[1]= atof(p);
}


int	parse_obj(const char *name, bf_container *ptr)
{
	char c, ce, *p, *q;
	int
		ival, itval, icx, itest, isurf,
		ctv, ctn, ctt,
		ctf, cti, cts,
		csts, csti, cstd;
	long xval;
	shurt *facefill, *facehold, *facen, *facet, *facel ;
	svector
		*vpcur, *vncur ;
	float *vtcur;
	bf_surface *esurf, *ematch;
	FILE * flib ;

	fin= fopen(name, "r");
	if (NULL == fin)
		{ printf(" >> unable to open %s\n", name);  RE }

	ctv= 0, ctn= 0, ctt= 0, ctf= 0, cti= 0, cts= 0;
	while (! rdline())
	{
		p= sline;  P_space
		c= *(p ++);
		switch (c)
		{
			case '#' :
				if (! strncmp(p, "#surf", 5)) cts ++ ;
				break ;
			case 'f':
				P_word
				icx= 0;
				while (*p) { P_word  icx ++; }
				ctf ++, cti += 4 * icx +3;
				break;
			case 'm':
				if (! strncmp(p, "tllib", 5))
				{
					P_word  P_end ;
					if (flib= fopen(q, "r"))
					{
						while (fgets(sline, sizeof(sline), flib))
							if (! strncmp(sline, "newmtl", 6)) cts ++ ;
						fclose(flib) ;
					}
				}
				break ;
			case 'u':
				cti ++;
				break;
			case 's':
				if (p[0] == 'u') cts ++;
				break;
			case 'v':
				c= *p;
				if ((' ' == c) || ('\t' == c)) ctv ++;
					else if ('n' == c) ctn ++;
					else if ('t' == c) ctt ++;
				break;
		}
	}

	// printf("parse [%d %d %d - %d %d - %d]\n", ctv, ctn, ctt, ctf, cti, cts);
	ptr-> vptr= malloc(ctv * sizeof(svector)),
	ptr-> nptr= malloc(ctn * sizeof(svector)),
	ptr-> uptr= malloc(ctt * 2 * sizeof(float));
	ptr-> sptr= malloc(cts * sizeof(bf_surface));
	ptr-> fptr= malloc(cti * sizeof(us));

	ctv= 1, ctn= 1, ctt= 1,
	cti= 0, cts= 0;
	csts= 0, csti= 0, cstd= 0;
	vpcur= (svector *) ptr-> vptr,
	vncur= (svector *) ptr-> nptr,
	vtcur= (float *) ptr-> uptr,
	esurf= (bf_surface *) ptr-> sptr;
	facefill= (us *) ptr-> fptr;

#define	isINDEX(uu)	(isdigit(c) || ('+' == c) || ('-' == c))

	isurf= 0;
	fin= fopen(name, "r");
	while (! rdline())
	{
		p= sline;  P_space
		c= *(p ++);
		switch (c)
		{
			case 'f':
				P_space
				facehold= facefill, facefill += 3;
				icx= 0, ival= 0;
				q= p;
				while (* p)
					{ P_word  icx ++; }
				p= q;
				while ((c= *p) && isINDEX(c)) p ++;
				if ('/' == c)
				{
					p ++, c= *p;
					if (isINDEX(c))
					{
						ival |= 4;
						while ((c= *p) && isINDEX(c)) p ++;
					}
					if ('/' == c)
						{ ival |= 2; }
				}
				p= q;
				cti += 3 + icx;
				facel= facefill + icx;
				if (ival & 4)
				{
					cti += icx;
					facet= facel, facel += icx;
				}
				if (ival & 2)
				{
					cti += icx;
					facen= facel, facel += icx;
				}
					else cti ++;

				while (*p)
				{
					xval= atoi(p);
						if (xval < 0) { xval += ctv; }
					if (xval < 1)
						{ printf(" >> f/v err\n");  RE }
					*(facefill ++)= (unsigned short) xval;
					while ((c= *p) && isINDEX(c)) p ++;
					if ('/' != c) { P_word  continue; }
					p ++, c= *p;
					if ('/' != c)
					{
						itval= atoi(p);
							if (itval < 0) itval += ctt;
						if (itval < 0) { printf(" >> f/t err\n");  RE }
						*(facet ++)= itval;
						while ((c= *p) && isINDEX(c)) p ++;
					}
					else
						{ itval= 0; }
					if ('/' == c)
					{
						p ++, c= *p;
						if (c && isINDEX(c))
						{
							itest= atoi(p);
								if (itest < 0) itest += ctn;
							if (itest < 1) { printf(" >> f/n err\n");  RE }
							*(facen ++)= itest;
						}
					}
					P_word

				}

				facehold[0]= (ival) ? (ival -1) : 0,
				facehold[1]= isurf,
				facehold[2]= icx;

				csts+= icx;
				if (ival)
				{
					csti += icx -3,
					facefill= facel;
				}
				if (!(ival & 2))
					{ facefill ++; }

				break;
			case 'm':
				if (! strncmp(p, "tllib", 5))
				{
					P_word  P_end ;
					if (flib= fopen(q, "r"))
					{
						int ctlib= surflib(flib, esurf) ;
						cts += ctlib, esurf += ctlib ;
						fclose(flib) ;
					}
				}
				break ;
			case 's':
				if ('u' != *p)
					break;
				P_word
				if (surfparse(p, esurf, (bf_surface *) ptr-> sptr))
					{ esurf ++, cts ++; }
				break;
			case '#':
				if (! strncmp(p, "#surf", 5))
				{
					P_word
					if (surfhack(p, esurf))
						{ esurf ++, cts ++; }
				}
				break ;
			case 'u':
				P_word  P_end
				if (! strncmp(q, "jack", 4))
					break ;
				for (ival= 0, ematch= (bf_surface *) ptr-> sptr;
					((ematch != esurf) && strcmp(ematch-> name, q)); ematch ++, ival ++) ;
				if (ematch == esurf)
					{ printf(" >> unable to match surface %s\n", q);  RE }
				isurf= ival;
				break;
			case 'v':
				ce= *(p ++);  P_space
				if (' ' == ce)
				{
					vecparse(p, vpcur);
					vpcur ++, ctv ++;
				}
				else
				if ('n' == ce)
				{
					vecparse(p, vncur);
					vncur ++, ctn ++;
				}
				else
				if ('t' == ce)
				{
					vec2parse(p, vtcur);
					vtcur += 2, ctt ++;
				}

				break;
		}
	}

xparse_final:
	ptr-> vcount= ctv -1,
	ptr-> ncount= ctn -1;
	ptr-> ucount= ctt -1;
	ptr-> scount= cts,
	ptr-> fcount= cti;
	ptr-> timesize= 0;
	return 0;
}

