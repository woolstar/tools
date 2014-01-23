// vek_implement.h -*- C++ -*-

#ifndef _CTL_VECTORIMPL
#define _CTL_VECTORIMPL	1

#include <typeinfo>
#include <utility>
#include <cstring>

namespace ctl
{
	namespace __detail
	{
	} ;

	template <class T>
		vector<T>::vector(size_t asize)
		{
			// guess at how large N Base are going to be + ctrl ;
		}

	// internal 

	template <class T>
		void	vector<T>::relocate( data * dstart, data *dlimit, data * zdest )
		{
			std::memmove(zdest, dstart, dlimit - dstart) ;
		}

} ;

#endif

