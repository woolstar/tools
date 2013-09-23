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
			buffer(void *, size_t) { }
			buffer(const void *, size_t) { }
			buffer(const char *) { }

			void *	getbuf(void) const { return rawbuffer_ ; } 
			size_t	getsize(void) const { return datasize_ ; }

		protected:
			void * rawbuffer_ { nullptr } ;
			size_t	datasize_ { 0 } ;
	} ;

	class	build_base
	{
		public:
			build_base(unsigned char * aptr, size_t amax) : buffer(aptr, 0), fill_(aptr), limit_(aptr + amax) { }
			virtual ~ build_base() ;

		protected:
			unsigned char * fill_, * limit_ ;
	} ;

	template <class ExpansionTrait, typename PrimativeStorage = unsigned char>
		class	build_methods : public build_base, public ExpansionTrait
		{
			public:
				build_methods &	add(const buffer &) ;
				build_methods &	add(const char * astr) ;

				build_methods &	add_u8(unsigned char) ;
				build_methods &	add_u16(unsigned short) ;
				build_methods &	add_u32(unsigned) ;
				build_methods &	add_u64(unsigned long long int) ;

				build_methods &	chomp() ;
				build_methods &	terminate() ;

			protected:
				template <int abits> friend void add(build &, unsigned int aval) ;

				unsigned char * fill_, * limit_ ;
		} ;


	template <int abits> void add(build_methods &, unsigned int aval) ;

	template <> void add<8>(build_methods & abuf, unsigned int aval) { abuf.add_u8( aval) ; }
	template <> void add<16>(build_methods & abuf, unsigned int aval) { abuf.add_u16( aval) ; }
	template <> void add<32>(build_methods & abuf, unsigned int aval) { abuf.add_u32( aval) ; }

		// traits

			// keep buffer restricted to initial size
	class	build_fixed : virtual public build_base
		{ protected: bool expand(int) { return false ; } } ;

			// exception on buffer overflow
	class	build_strict : virtual public build_base
		{ protected: bool expand(int) { throw std::length_error("exceeded buffer") ; } } ;

			// attempt to reallocate larger buffer each time
	class	build_expand : virtual public build_base
		{ protected: bool expand(int) ; } ;

	template <int asize, typename aTYPE = unsigned char>
		class build_static : public build_methods<build_fixed>
		{
			public:
				build_static() : build_base(reinterpret_cast<unsigned char *>(storage_), sizeof(storage_)) { }
				build_static(const buffer &) ;
				build_static(char const * astr, size_t, char aencoding) ;

			protected:
				bool	expand(int) { return false ; }	// no expansion

				aTYPE	storage_[asize] ;
		} ;

	//

	class	build_managed : public build_methods<build_expand>
	{
		public:
			build_managed(size_t asize = 1024) ;
			build_managed(const buffer & ) ;

			build_managed(build_managed &&) ;
			build_managed(build_managed &) = delete ;	// use the buffer constructor instead

			build_managed& operator=(build_managed &&) ;

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

