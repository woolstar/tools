// <safety_impl.h>     -*- C++ -*-

/** @file include/bits/safety_impl.h
  *
  */

#ifndef _MISC_SAFETY_IMPLEMENT
#define _MISC_SAFETY_IMPLEMENT 1

namespace vb
{

template <typename... Args>
safety_once<Args...>::safety_once(fn_type fn)
			: m_func( fn ),
			  m_live( true )
{ }

template <typename... Args>
safety_once<Args...>::safety_once( safety_once && src )
			: m_func( src.m_func ),
			  m_live( src.m_live )
{ }

template <typename... Args>
safety_once<Args...> &
safety_once<Args...>::operator=(safety_once && src) noexcept
{
	m_func= src.m_func ;
	m_live= src.m_live ;
}

	//


} 

#endif

