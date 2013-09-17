#ifndef LTL_BUFFER_
#define LTL_BUFFER_ 1

#include <cstddef>

namespace ltl
{
	class	buffer
	{
		public:
			buffer() { }
			buffer(void *, size_t) { }
			buffer(const void *, size_t) { }
			buffer(const char *) { }

			virtual ~ buffer() { }

			void *	getbuf(void) const { return rawbuffer_ ; } 
			size_t	getsize(void) const { return size_ ; }

		protected:
			void * rawbuffer_ { nullptr } ;
			size_t	size_ { 0 } ;
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


	class	build_fixed : public build
	{
		public:
			build_fixed(int asize = 256) ;
			build_fixed(const buffer &) ;
			build_fixed(const char *, size_t, char) ;

			~ build_fixed() ; // ... deallocate

		protected:
			bool	expand(int) { return false ; }	// no expansion
			// smart pointer holder
	} ;

	class	build_strict : public build_fixed
	{
		protected:
			bool	expand(int) { throw 1 ; }	// exception on buffer overflow
	} ;

		/** not supported in GCC 4.6 */
	namespace literal
	{
		build_fixed operator"" _Hex(char const * astr, size_t alen) 	// hex array to buffer
		{
			return build_fixed(astr, alen / 2, 'h') ;
		}
	} ;

	class	build_expand : public build
	{
		public:
			build_expand(int ainitialsize = 1024) ;
			build_expand(const buffer &) ;

			~ build_expand() ;  // ... deallocate

		protected:
			// smart pointer holder
	} ;

	template <int asize, typename aTYPE = char>
		class build_static : public build
		{
			public:
				build_static() : build(storage_, sizeof(storage_)) { }

			protected:
				aTYPE	storage_[asize] ;
		} ;

} ;

#endif

