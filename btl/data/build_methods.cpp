
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
build_managed::build_managed(size_t asize) : build_managed( size_multiple( asize), eChecked ) { }

build_managed::build_managed(const buffer & acopy) : build_managed( acopy.size())
{
	copy( acopy) ;
}

build_managed::build_managed(size_t asize, btl::build_managed::SizeCheck)
{
	storage_= btl::make_unique<sized_storage [] >(asize) ;
	// storage_.reset( ( tmpptr = new sized_storage [bufsize] ) ) ;

	sized_storage * tmpptr = storage_.get() ;

	rawbuffer_= far_= tmpptr ;
	limit_ = tmpptr + asize ;
}

	// move constructor and move assignment
build_managed::build_managed(build_managed && asrc)
{
	size_t bufsize = (asrc.limit_ - & ( asrc.storage_ [0] ) ) ;

		// take over settings from source
	storage_ = std::move( asrc.storage_ ) ;
	buffer::swap( asrc) ;
	limit_ = asrc.limit_ ;

	asrc.limit_ = nullptr ;
}

build_managed &  build_managed::operator=(build_managed && asrc)
{
	if ( this != & asrc )
	{
		size_t bufsize = (asrc.limit_ - & ( asrc.storage_ [0] ) ) ;

		* this= nullbuf ;
			// take over settings from source
			// unique_ptr<>::storage_ takes care of destroying any previous data
		storage_ = std::move( asrc.storage_ ) ;
		buffer::swap( asrc) ;

		limit_ = asrc.limit_, asrc.limit_= nullptr ;
	}

	return * this ;
}

