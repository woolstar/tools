#ifndef BTL_BUFFER_
#define BTL_BUFFER_ 1

#include <cstddef>
#include <stdexcept>
#include <memory>
#include <string.h>

namespace btl
{
	typedef	unsigned char	sized_storage;

	class	build_base : public buffer
	{
		public:
			build_base(unsigned char * aptr, size_t amax) : buffer(aptr, 0), fill_(aptr), limit_(aptr + amax) { }
			virtual ~ build_base() ;

		protected:
			build_base() {}
			sized_storage * fill_, * limit_ ;

			virtual sized_storage * getbuffer(void) = 0 ;
			virtual sized_storage const *	getbuffer(void) const = 0 ;

			void	copy(const buffer &) ;
	} ;

		// traits

			// keep buffer restricted to initial size
	class	expand_fixed : virtual public build_base
		{ protected: void expand(int) { } } ;

			// exception on buffer overflow
	class	expand_strict : virtual public build_base
		{ protected: void expand(int) { throw std::length_error("exceeded buffer") ; } } ;

			// attempt to reallocate larger buffer each time
	class	expand_alloc : virtual public build_base
	{
		protected:
			void expand(int) ; 
			std::unique_ptr<sized_storage []>	storage_ ;

			sized_storage *	getbuffer(void) { return & ( storage_[0] ) ; }
			sized_storage const *	getbuffer(void) const { return & ( storage_[0] ) ; }
	} ;

			// add methods
	template <class TRAIT>
		class	build_methods : virtual public build_base, public TRAIT
		{
			public:
				build_methods<TRAIT> &	add(const buffer & abuf) 
					{ test( abuf.size()) ;  copy( abuf) ;  return * this ; }
				build_methods<TRAIT> &	add(const char * astr)
				{
					int alen= strlen( astr) ;
					test( alen) ;
					copy( buffer( astr, alen)) ;
					return * this ;
				}

				build_methods<TRAIT> &	add_u8(unsigned char) ;
				build_methods<TRAIT> &	add_u16(unsigned short) ;
				build_methods<TRAIT> &	add_u32(unsigned) ;
				build_methods<TRAIT> &	add_u64(unsigned long long int) ;

				build_methods<TRAIT> &	chomp() 
					{ if ( datasize_ ) { datasize_ -- ; fill_ -- ; } return * this; } 
				build_methods<TRAIT> &	terminate()
					{ if ( fill_ < limit_ ) { * fill_= '\0' ; }  return * this ; }

				size_t	remaining(void) const { return limit_ - fill_ ; }

			protected:
				build_methods() {}	// gets ignored

				void	test(size_t) ;
		} ;

	// template <> void add<8>(build_methods<> & abuf, unsigned int aval) { abuf.add_u8( aval) ; }
	// template <> build_methods<> & add<16>(unsigned int aval) { return add_u16(aval) ; }
	template <class TRAIT> void	build_methods<TRAIT>::test(size_t asize)
	{
		if ( ( fill_ + asize ) > limit_ )
			{ TRAIT::expand( asize - ( limit_ - fill_ )) ; }
	}

	template <int asize>
		class build_static : public build_methods<expand_fixed>
		{
			public:
				build_static() : build_base(reinterpret_cast<sized_storage *>(storage_), sizeof(storage_)) { }
				build_static(const buffer & abuf) : build_static() { add( abuf) ; }
				// build_static(char const * astr, size_t, char aencoding) ;

			protected:
				bool	expand(int) { return false ; }	// no expansion
				sized_storage *	getbuffer(void) { return storage_ ; }
				sized_storage const *	getbuffer(void) const { return storage_ ; }

				sized_storage	storage_[asize] ;
		} ;

	//

	class	build_managed : public build_methods<expand_alloc>
	{
		public:
			build_managed(size_t asize = 1024) ;
			build_managed(const buffer & ) ;

			build_managed(build_managed &&) ;
			build_managed(build_managed &) = delete ;	// use the buffer constructor instead

			build_managed& operator=(build_managed &&) ;
	} ;

	class	build_fixed : public build_methods<expand_fixed>
	{
		public:
			build_fixed(size_t asize = 1024) ;
			build_fixed(const buffer & ) ;
			build_fixed(build_managed &) ;

			build_fixed(build_managed &&) ;
			build_fixed& operator=(build_fixed &&) ;

		protected:
			std::unique_ptr<sized_storage *>	storage_ ;
	} ;
} ;

#endif

