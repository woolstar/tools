#ifndef _DATA_FORMAT_H
#define _DATA_FORMAT_H	1

#include <stdio.h>

namespace btl
{
	template <class _uObj, int N = 64>
		build_static<N>	format( const char * afmt, _uObj data )
					{
						build_static<N>	tmp ;

						tmp.print( afmt, data ) ;
						return tmp ;
					}
} ;

#endif

