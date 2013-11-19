// format channel -*- C++ -*-

#ifndef _BTL_FMTCHAN_H
#define _BTL_FMTCHAN_H 1

#include	<buffer>

namespace btl
{
	class	channel_text_scanf
	{
		public:
			channel_text_scanf( unsigned int eolvec = ( ( 1 << '\n' ) | (1 << '\r') ) ) ;

		protected:
			void	reset(void) ;
			void	scan( const buffer &, scanner<> & ) ;

			const static int	kEOLMax = 31 ;
			const unsigned int	eolv_ ;
	} ;

	class	channel_packet_scanf
	{
		public:
			channel_packet_scanf() { reset() ; }

		protected:
			void	reset(void) ;
			void	scan( const buffer &, scanner<> & ) ;

			bool	in_header = true ;
			int		msg_code, msg_length ;

			const static int	kHeaderLen = 4 ;	// small header
	} ;

	template <class ScannerType, class StorageType = build_static<1024>>
		class	channel_buffered_scanner : public channel_if, public ScannerType
		{
			public:
				template< typename... Args >
					channel_buffered_scanner( feeder & afeed, Args&&... args)
						: formatter_(std::forward<Args>(args)...), channel_if( afeed ) { }

				void	data( const buffer & adata ) ;

			protected:
				void	do_msg( void)
						{ pass( buffer_ ) ;  buffer_.reset() ;  formatter_.reset() ; }

				ScannerType formatter_ ;
				StorageType	buffer_ ;
		} ;

	template <class ScanT, class StorT>
		void	channel_buffered_scanner<ScanT, StorT>::data( const buffer & adata )
		{
			scanner<>	scanref( adata) ;
			size_t use_sz, fill_sz ;

			while ( scanref )
			{
				if ( ( use_sz= formatter_.scan( scanref) ) > 0 ) {
					fill_sz= buffer_.size() ;
					buffer_.add( scanref, use_sz ) ;
					if ( fill_sz == buffer_.size() ) // unable to pack any more data into buffer_
						{ if ( fill_sz ) { do_msg() ; } else { signal( 0 ) ; } }
				}
				else
				if ( ! use_sz ) { do_msg() ; }
					else { signal( use_sz ) ; }	// error
			}
		}

	// great place to use template metaprogramming to do storage version that handles exceptions

} ;

#endif

