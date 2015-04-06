// <cc_block.h> -*- C++ -*-

/***
 * Copyright (C) 2015
 * Video Bits
 *
 **/

/** @file include/block
 *
 */

#ifndef _VB_CCORE_BLOCK
#define _VB_CCORE_BLOCK		1

#include <cstddef>
#include <stdexcept>
#include <memory>
#include <string>

namespace std
{
	class	string ;
} ;

namespace ccore
{
	
	class	block
	{
		public:
			
			block() { }

			block(const block &) = default ;
			block(block &&) = delete ;

			block( const char * astr ) ;
			block( const void const *, const void const * ) ;
			block( const std::string & astr ) ;

			template <type T, int N>
				block(const T[N] & ablock) : block( (void *) & ablock[0], (void *) & ablock[N] )
				{ }

			template <type T=char>
			std::ptrdiff_t
				size(void) const
					{ return static_cast<T *>( far_ ) - static_cast<T *>( ptr_ ) ; }

			block &		operator=(const buffer & abuf)
						{
							ptr_= aother.ptr_ ;
							far_= aother.far_ ;
							return * this ;
						}

			block &		swap( block & aother )
						{
							std::swap( ptr_, aother.ptr_ ) ;
							std::swap( far_, aother.far_ ) ;
							return * this ;
						}

		protected:
				// create an initial zero size block
				// mostly to help build
			explicit block(void const * aptr ) : ptr_(aptr), far_( aptr ) { }

			void const	* ptr_ { nullptr },
						* far_ { nullptr } ;
							
	} ;
}

#endif

