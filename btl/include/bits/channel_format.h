// format channel -*- C++ -*-

#ifndef _BTL_FMTCHAN_H
#define _BTL_FMTCHAN_H 1

#include	<buffer>

namespace btl
{
	template <char...> struct CtrlVec ;
	template <char c> struct CtrlVec<c>
		{ static_assert( ( c < 0x20 ), "Only for control characters" ) ;  enum { mask = ( 1 << c ) } ; } ;
	template <char c, char... cs> struct CtrlVec<c, cs...>
		{ static_assert( ( c < 0x20 ), "Only for control characters" ) ;  enum { mask = ( 1 << c) | CtrlVec<cs...>::mask } ; } ;

	class	text_scanf
	{
		public:
			text_scanf( unsigned int eolvec = ( CtrlVec<'\n','\r'>::mask ) ) : eolv_( eolvec ) { reset() ; }

			void	reset(void) { curv_ = 0 ; }
			size_t	scan( const buffer &, scanner<> &) ;

		protected:
			unsigned int	curv_ ;

			const static int	kEOLMax = 31 ;
			const unsigned int	eolv_ ;
	} ;

	class	packet_scanf
	{
		public:
			packet_scanf() { reset() ; }

			void	reset(void) ;
			size_t	scan( const buffer &, scanner<> & ) ;

		protected:
			void	decode( const buffer & ) ;

			bool	in_header = true ;
			int		msg_code, msg_length ;

			const static int	kHeaderLen = 4 ;	// small header
	} ;

	template <class ScannerType, class StorageType = build_static<1024>>
		class	channel_buffered_scanner : public channel_if
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

	////
	// - details

	template <class ScanT, class StorT>
		void	channel_buffered_scanner<ScanT, StorT>::data( const buffer & adata )
		{
			scanner<>	scanref( adata) ;
			size_t use_sz, fill_sz ;

			while ( scanref )
			{
				if ( ( use_sz= formatter_.scan( buffer_, scanref) ) > 0 ) {
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

	static_assert( CtrlVec<'\0', '', ''>::mask == 0x7, "") ;
} ;

#endif

