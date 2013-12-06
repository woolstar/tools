#include "basic.h"
#include "tscene.h"

int	tart_metatest(tscene * src)
{
	int istep, ico;
	tscene_element * estep;

	for (istep= src-> elem.count, ico= 0, estep= src-> elem.list;
		(istep --); estep ++)
	{
		if (strchr(estep-> name, ':') && (_E_OBJECT == estep-> type)) ico ++;
	}

	return (ico > 0);
}

int	tart_selectchar(tscene * dest, tscene * src, char * cname)
{
	char namemark[64], * p;
	int istep, icount, ic, nlen;
	tscene_element
		* estep, * nblock, * ncreate,
		** ublock, ** ucreate;

	strncpy(namemark, cname, sizeof(namemark));
	strcat(namemark, ":");
	nlen= strlen(namemark);

	for (istep= src-> elem.count, icount= 0, ic= 0, estep= src-> elem.list; 
		(istep --); estep ++)
	{
		if (! strchr(estep-> name, ':'))
		{
			icount ++;
			if (_E_OBJECT == estep-> type) ic ++;
			continue ;
		}

		if (! strncmp(estep-> name, namemark, nlen)) 
		{
			icount ++;
			if (_E_OBJECT == estep-> type) ic ++;
		}
	}

	if (! icount || ! ic)
	{
		dest-> elem.count= 0,
		dest-> elem.list= NULL;
		dest-> set.count= 0,
		dest-> set.list= NULL;

		return 1;
	}

	nblock= EMEM(icount, tscene_element);
	ublock= EMEM(ic, tscene_element *);
	if ((NULL == nblock) || (NULL == ublock))
		return 3;

	for (istep= src-> elem.count, estep= src-> elem.list,
			ncreate= nblock, ucreate= ublock; 
		(istep --); estep ++)
	{
		p= strchr(estep-> name, ':');
		if (NULL == p)
		{
			memcpy(ncreate, estep, sizeof(tscene_element));
			if (_E_OBJECT == estep-> type)
				*(ucreate ++)= ncreate;
			ncreate ++;
			continue ;
		}

		if (strncmp(estep-> name, namemark, nlen)) 
			continue ;

		memcpy(ncreate, estep, sizeof(tscene_element));
		p ++;
		strcpy(ncreate-> name, p);

		if (_E_OBJECT == estep-> type)
			*(ucreate ++)= ncreate;
		ncreate ++;
	}

	memcpy(dest, src, sizeof(tscene));

	dest-> elem.count= icount,
	dest-> elem.list= nblock;
	dest-> set.count= ic,
	dest-> set.list= ublock;

	return 0;
}

