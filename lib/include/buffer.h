#ifndef BTL_BUFFER_
#define BTL_BUFFER_ 1

#include <cstddef>
#include <stdexcept>
#include <memory>

namespace btl
{
	class	buffer
	{
		public:
			buffer() { }
			buffer(void * aptr, size_t n) : rawbuffer_(aptr), datasize_(n) { }
			buffer(char *) ;

			void const *	operator*(void) const { return rawbuffer_ ; }
			size_t	size(void) const { return datasize_ ; }

		protected:
			void * rawbuffer_ { nullptr } ;
			size_t	datasize_ { 0 } ;
	} ;

	class	build_base : public buffer
	{
		public:
			build_base(unsigned char * aptr, size_t amax) : buffer(aptr, 0), fill_(aptr), limit_(aptr + amax) { }
			virtual ~ build_base() ;

		protected:
			build_base() {}
			unsigned char * fill_, * limit_ ;
	} ;

		// traits

			// keep buffer restricted to initial size
	class	expand_fixed : virtual public build_base
		{ protected: bool expand(int) { return false ; } } ;

			// exception on buffer overflow
	class	expand_strict : virtual public build_base
		{ protected: bool expand(int) { throw std::length_error("exceeded buffer") ; } } ;

			// attempt to reallocate larger buffer each time
	class	expand_alloc : virtual public build_base
	{
		protected:
			bool expand(int) ; 
			std::unique_ptr<unsigned char *>	storage_ ;
	} ;

			// add methods
	template <class ET, typename PSto = unsigned char>
		class	build_methods : virtual public build_base, public ET
		{
			public:
				build_methods<ET,PSto> &	add(const buffer &) ;
				build_methods<ET,PSto> &	add(const char * astr) ;

				build_methods<ET,PSto> &	add_u8(unsigned char) ;
				build_methods<ET,PSto> &	add_u16(unsigned short) ;
				build_methods<ET,PSto> &	add_u32(unsigned) ;
				build_methods<ET,PSto> &	add_u64(unsigned long long int) ;

				build_methods<ET,PSto> &	chomp() ;
				build_methods<ET,PSto> &	terminate() ;

				size_t	remaining(void) const { return limit_ - fill_ ; }

			protected:
				build_methods() {}	// gets ignored
		} ;

	// template <> void add<8>(build_methods<> & abuf, unsigned int aval) { abuf.add_u8( aval) ; }
	// template <> build_methods<> & add<16>(unsigned int aval) { return add_u16(aval) ; }

	template <int asize, typename aTYPE = unsigned char>
		class build_static : public build_methods<expand_fixed>
		{
			public:
				build_static() : build_base(reinterpret_cast<unsigned char *>(storage_), sizeof(storage_)) { }
				build_static(const buffer & abuf) : build_static() { add( abuf) ; }
				// build_static(char const * astr, size_t, char aencoding) ;

			protected:
				bool	expand(int) { return false ; }	// no expansion

				aTYPE	storage_[asize] ;
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
			std::unique_ptr<unsigned char *>	storage_ ;
	} ;

#if 0
	// work in progress

		/** not supported in GCC 4.6 */
	namespace literals
	{
		constexpr build_static<>  operator"" _Hex(char const * astr, size_t alen) 	// hex array to buffer
		{
			return build_static<(alen/2)-1, unsigned char>(astr, (alen / 2) -1, 'h') ;
		}
	} ;
#endif

} ;

#endif

