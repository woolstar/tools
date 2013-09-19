#ifndef LTL_BUFFER_
#define LTL_BUFFER_ 1

#include <cstddef>
#include <stdexcept>
#include <memory>

namespace ltl
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

	class	build : public buffer
	{
		public:
			build(unsigned char * astorage, int amax) :
				buffer( astorage, 0), fill_(astorage), limit_(astorage + amax) { }

			build &	add(const buffer &) ;
			build &	add(const char * astr) ;

			build &	add_u8(unsigned char) ;
			build &	add_u16(unsigned short) ;
			build &	add_u32(unsigned) ;
			build &	add_u64(unsigned long long int) ;

			build &	chomp() ;
			void	terminate() ;

		protected:
			virtual bool	expand(int aspace) = 0 ;

			template <int abits> friend void add(build &, unsigned int aval) ;

			unsigned char * fill_, * limit_ ;
	} ;


	template <int abits> void add(build &, unsigned int aval) ;

	template <> void add<8>(build & abuf, unsigned int aval) { abuf.add_u8( aval) ; }
	template <> void add<16>(build & abuf, unsigned int aval) { abuf.add_u16( aval) ; }
	template <> void add<32>(build & abuf, unsigned int aval) { abuf.add_u32( aval) ; }

	template <int asize, typename aTYPE = unsigned char>
		class build_static : public build
		{
			public:
				build_static() : build(reinterpret_cast<unsigned char *>(&storage_[0]), sizeof(storage_)) { }
				build_static(const buffer &) ;
				build_static(char const * astr, size_t, char aencoding) ;

			protected:
				bool	expand(int) { return false ; }	// no expansion

				aTYPE	storage_[asize] ;
		} ;

	//

	class	build_resource : public build
	{
		public:
			build_resource(size_t asize = 1024) ;
			build_resource(const buffer & ) ;

			build_resource(build_resource &&) ;
			build_resource(build_resource &) = delete ;

		protected:
			std::unique_ptr<unsigned char *>	storage_ ;
	} ;

		// traits

			// keep buffer restricted to initial size
	class	build_fixed : virtual public build
		{ protected: bool	expand(int) { return false ; } } ;

			// exception on buffer overflow
	class	build_strict : virtual public build_fixed
		{ protected: bool	expand(int) { throw std::length_error("exceeded buffer") ; } } ;

	class	build_expand : public build
	{
		public:
			build_expand(int ainitialsize = 1024) ;
			build_expand(const buffer &) ;

			~ build_expand() ;  // ... deallocate

		protected:
			// smart pointer holder
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

