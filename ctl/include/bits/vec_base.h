namespace ctl
{
	namespace __detail
	{
		class vector_base
		{
			void	size(size_t) ;
			void	shrink(size_t) ;

				// storage
			size_t	use_ = 0, total_ = 0 ;
			std::unique_ptr<unsigned char []>	storage_ ;
		} ;
	} ;
} ;

