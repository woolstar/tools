
// <safety_once.h>    -*- C++ -*-

/***
 * Copyright (C) 2016
 * Video Bits
 *
 **/

/** @file include/safety_once.h
 *
 */


#ifndef _MISC_SAFETYONCE
#define _MISC_SAFETYONCE 1

#include <functional>

namespace vb
{

template <typename... Args>
class	safety_once
{
	public:
		typedef std::function< void(Args...) > fn_type ;

		safety_once(fn_type) ;
		safety_once(safety_once &&) ;
		safety_once(const safety_once &) = delete ;

		safety_once & operator=(safety_once &&) noexcept ;
		safety_once & operator=(const safety_once &) = delete ;
		
		void operator()(Args...) const ;

	protected:
		std::function< void(Args...) >	m_func ;
		mutable bool                    m_live ;
} ;

template <typename... Args>
class	strict_once : public safety_once<Args...>
{
	public:
		~ strict_once() ;

		typedef std::function< void(Args...) > fn_type ;
		void operator()(Args...) const ;

} ;

}

#include <bits/safety_impl.h>

#endif

