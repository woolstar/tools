   /****
      Copyright 1996 Twin Forces Entertainment
      All Rights Reserved
      */

#ifndef	TFBASIC_IO
#	define	TFBASIC_IO	1

#define	sM_iswhite(c) ((' ' == c) || ('\t' == c) || ('\n' == c) || ('\r' == c))
#define	sM_start(p, xxx) p= xxx;  sM_white(p) ;
#define	sM_white(p) while ((c= *p) && sM_iswhite(c)) p ++
#define sM_nwhite(p) while ((c= *p) && ! sM_iswhite(c)) p ++
#define sM_skip(p) sM_nwhite(p); sM_white(p)
#define sM_word(p) sM_nwhite(p);  if (c) { *(p ++)= '\0';  sM_white(p); }
#define sM_get(u, p) q= u;  while ((c= *p) && ! sM_iswhite(c)) { *(q ++)= c;  p++; } *q='\0';  sM_white(p)

#define	P_start	 p= ixline; P_space
#define P_space	 while (((c= *p)== ' ') || (c== '\t')) p++; if ((c=='\n') || (c=='\r')) *p= 0;
#define P_word while (((c= *p)!= ' ') && (c!= '\t') && (c!= '\n') && (c!= '\r') && c) p++; P_space
#define PQ_cap q=p; while (((c= *q)!= ' ') && (c!= '\t') && (c!= '\n') && (c!= '\r') && c) q++; *q= 0;
#define P_end q= p; while (((c= *p)!= ' ') && (c!= '\t') && (c!= '\n') && (c!= '\r') && c) \
			p++; *(p++)= 0; P_space
#define	P_zEOL  if (q= strchr(p, '\r')) *q= 0;  if (q= strchr(p, '\n')) *q= 0;
#define P_cpyword(x)	q= x; while (((c= *p)!= ' ') && (c!= '\t') && (c!= '\n') && (c!= '\r') && c) \
			{ p++; *(q++)= c; } P_space
#define	P_fixword(x)	q= x; while (c= *q) { if (c=='*') *q= ' '; q++; }
#define P_unfix(x)	q= x; while (c= *q) { if (c==' ') *q= '*'; q++; }
#define Q_lower		ql= q; while (c= *ql) { if ((c>='A')&&(c<='Z')) *ql= c+'a'-'A'; ql++; }
#define Q_is(x)		(!strcmp(q, x))
#define Qn_is(x, y)	(!strncmp(q, x, y))
#define Pn_is(x, y)	(!strncmp(p, x, y))
#define Qitem(x)	case *x: if Q_is(x)
#define DIE		exit (-1);
#define	READLN		fgets(ixline, 128, f)
#define	READIN(F)	fgets(ixline, 128, F)
#define	SI		sizeof(short)
#define SD		sizeof(dussel)
#define creFILENAME(x,y,z)	strcpy(x,y); strcat(x,z);
#define	IO_NEEDED	register char c, *p, *q;
#define GETS(x)		fgets(ixline, 128, x)
#define	OUTP(x)		{ printf(x);  fflush(stdout); }
#define	S_word		P_start P_end  switch (*q)
#define MEM(a,b)	(b *) malloc(a * (long) sizeof(b))
#define QOTE		p++; while (1) \
			{  while (((c= *p)!= '\'') && (c!='\n') && (c!='\r') && c) \
			   { *(q++)= c, p++; } \
			   if (((c= *p)== '\'') && (p[1]== '\'')) { *(q++)= c, p++, p++; } \
			   else break; } *q= '\0';

#define	PZ_start	p= ixline; PZ_space
#define	PZ_space	while (sz && (((c= *p) == ' ') || (c == '\t'))) p ++, sz --;
#define PZ_word		while (sz && (((c= *p) != ' ') && (c!= '\t'))) p ++, sz --;  PZ_space
#define PZ_end		q= p;  while (sz && (((c= *p) != ' ') && (c!= '\t'))) p ++, sz --; \
					if (*p && sz) *(p ++)= 0, sz --;  PZ_space
#define QZ_wlen(n)	q= p;  while (sz && (((c= *p) != ' ') && (c!= '\t'))) p ++, sz --;  n= p -q;
#define	PZ_eol		while (sz && (((c= *p) == '\r') || (c == '\n'))) *(p ++)= '\0', sz --;

#define	PZ_strchr(z, c)	while (sz && (c != *p)) { sz --, p ++; }  if (sz) z= p; else z= NULL;
#define	PZ_strchr2(z, c1, c2)	while (sz && (c1 != *p) && (c2 != *p)) { sz --, p ++; }  if (sz) z= p; else z= NULL;

#endif

