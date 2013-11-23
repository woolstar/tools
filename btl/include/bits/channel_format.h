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

	enum ScanAction {
			eNone = 0, 
			eData,
			eComplete,
			eFault,
		} ;

	class	text_scanf
	{
		public:
			text_scanf( unsigned int eolvec = ( CtrlVec<'\n','\r'>::mask ) ) : eolv_( eolvec ) { reset() ; }

			void	reset(void) { curv_ = 0 ; }
			ScanAction	scan( const buffer &, scanner<>, size_t & ) ;

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
			ScanAction	scan( const buffer &, scanner<>, size_t & ) ;

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
			ScanAction scact ;
			size_t use_sz, fill_sz ;

			while ( scanref )
			{
				switch ( formatter_.scan( buffer_, scanref, use_sz) )
				{
					case eData:
						fill_sz= buffer_.size() ;
						buffer_.add( scanref, use_sz ) ;
						if ( fill_sz == buffer_.size() ) // unable to pack any more data into buffer_
							{ if ( fill_sz ) { do_msg() ; } else { signal( 0 ) ; } }
						break ;
					case eComplete:
						do_msg() ;
					case eNone:
						scanref += use_sz ;
						break ;
					case eFault:
						signal( eFault) ;
						break ;
				}
			}
		}

	// great place to use template metaprogramming to do storage version that handles exceptions

	static_assert( CtrlVec<'\0', '', ''>::mask == 0x7, "") ;
} ;

#endif

