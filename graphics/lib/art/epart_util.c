#include "basic.h"

	/*** translate
		> Decode a sequence word into a sprintf form.
		'fileheader*0644.tga' -> 'fileheader%04d.tga' + code=3 + offset=644
		'simplename.tga' -> 'simplename.tga' + code=1 + offset=0
	*/

#define	DETRNUM(xlen, xval) for (xlen= 0, xval= 0; ((c= *p) && isdigit(c)); p ++, xlen ++) \
		{ xval= 10 * xval + (c -'0'); }

int	tart_seqname(char *dest, int *offset, char *word)
{
	int ilen, ival;
	char *p, *q, c, *s;

	s=dest;
	p= strchr(word, '*');
	if (NULL == p)
	{
		strcpy(dest, word);
		*offset= 0;
		return 1;
	}

	q= strchr(p +1, '*');
	if (NULL != q)
	{
		strcpy(dest, word);
		*offset= 0;
		return 5;
	}

	q= word;
	while (q != p) { *(dest ++)= *(q ++); }
	p ++;
	DETRNUM(ilen, ival)

	if (ilen > 9) ilen= 9;
	*(dest ++)= '%', *(dest ++)= '0', *(dest ++)= ilen + '0', *(dest ++)= 'd';
	strcpy(dest, p);
	*offset= ival;

	return 3;
}

	/*** undo translate
		> recode the original sequence string that created the given the form and offset.
		'complexsample%05d.tga' + code=3 + offset=1022 -> 'complexsample*01022.tga'
		'easy.tga' + code=1 -> 'easy.tga'
	*/

void	tart_undo_seqname(char *dest, int offset, char *word)
{
	char *p, *q;

	p= strchr(word, '%');
	if (NULL == p)
	{
		strcpy(dest, word);
		return;
	}

	q= word;
	while (q != p) { *(dest ++)= *(q ++); }
	*(dest ++)= '*';
	sprintf(dest, q, offset);
}

void	tart_seqform(char *dest, char *tmpl, int code, int frame, int offset)
{
	char c, *p;
	int rl, rbase;
	char tmplt[8];

	switch (code)
	{
		case 3:
			sprintf(dest, tmpl, frame +offset -1);
			break;
		case 5:
			p= tmpl;
			strcpy(tmplt, "%00d");
			while (c= *(p ++))
			{
				if ('*' == c)
				{
					DETRNUM(rl, rbase)
					if (rl > 9) rl= 9;
					tmplt[2]= rl +'0';
					sprintf(dest, tmplt, frame +rbase -1);
					while (* dest) dest ++;
				}
					else *(dest ++)= c;
			}
			break;
		default:
			strcpy(dest, tmpl);
			break;
	}
}

void	tart_seqdirect(char * dest, char * src, int frame)
{
	char c, *p, *q, *r, tmpstr[16], tmpfmt[16];
	int rl, roff ;

	p= src, q= dest ;
	while (c= *(p ++))
	{
		if ('*' != c) { *(q ++)= c;  continue; }

		roff= atoi(p), rl= 0;
		if (! roff) roff= 1;
		while (isdigit(* p)) { p ++, rl ++; }

		if (rl > 1) sprintf(tmpfmt, "%%0%dd", rl);
			else strcpy(tmpfmt, "%d");
		sprintf(tmpstr, tmpfmt, frame + roff -1);
		r= tmpstr ;
		while (c= *(r ++)) *(q ++)= c;
	}

	*q= '\0';
}


