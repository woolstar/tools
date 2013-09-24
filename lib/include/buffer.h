#ifndef BTL_BUFFER_
#define BTL_BUFFER_ 1

#include <cstddef>
#include <stdexcept>
#include <memory>
#include <string.h>

namespace btl
{

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

