
#include <buffer>

const static int	buffer_multiple = 64 ;

	static size_t	size_multiple(size_t asize)
	{
		if ( asize & ( buffer_multiple -1 ) )
			{ asize= ( asize & ( ~ ( buffer_multiple - 1 ) ) ) + buffer_multiple ; }
		return asize ;
	}

	using btl::build_managed ;

	// constructors
build_managed::build_managed(size_t asize)
{
	size_t bufsize = size_multiple( asize) ;
	sized_storage * tmpptr ;

	storage_.reset( ( tmpptr = new sized_storage [bufsize] ) ) ;

	rawbuffer_= tmpptr ;
	fill_relocate( tmpptr) ;
	limit_ = tmpptr + bufsize ;
}

build_managed::build_managed(const buffer & acopy) : build_managed( acopy.size())
{
	copy( acopy) ;
}

	// move constructor and move assignment
build_managed::build_managed(build_managed && asrc)
{
	size_t bufsize = (asrc.limit_ - & ( asrc.storage_ [0] ) ) ;

		// take over settings from source
	storage_ = std::move( asrc.storage_ ) ;
	buffer::setup( & storage_[0], asrc.datasize_ ) ;
	fill_relocate( asrc.fill_get()) ;
	limit_ = asrc.limit_ ;

		// empty source
	asrc.buffer::setup( nullptr, 0) ;
	asrc.fill_relocate( nullptr) ;  limit_ = nullptr ;
}

build_managed &  build_managed::operator=(build_managed && asrc)
{
	if ( this != & asrc )
	{
		size_t bufsize = (asrc.limit_ - & ( asrc.storage_ [0] ) ) ;

			// take over settings from source
			// unique_ptr<>::storage_ takes care of destroying any previous data
		storage_ = std::move( asrc.storage_ ) ;
		buffer::setup( & storage_[0], asrc.datasize_ ) ;
		fill_relocate( asrc.fill_get()) ;
		limit_ = asrc.limit_ ;

			// empty source
		asrc.buffer::setup( nullptr, 0) ;
		asrc.fill_relocate( nullptr) ;  limit_ = nullptr ;
	}

	return * this ;
}

