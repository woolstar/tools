// vec_implement.h -*- C++ -*-

#ifndef _CTL_VECIMPL
#define _CTL_VECIMPL	1

namespace ctl
{

	// implementation
	// lots of ugly

	template <class T>
		template <class Tc, class... Args>
			void vector<T>::emplace_back( Args&&... arg )
			{
			}

} ;

#endif

