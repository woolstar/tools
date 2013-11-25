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
			bool	scan( build_if &, scanner<> &) ;

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
			bool	scan( build_if &, scanner<> &) ;

		protected:
			void	decode( const buffer & ) ;

			bool	in_header = true ;
			int		msg_code, msg_length ;

			const static int	kHeaderLen = 4 ;	// small header
	} ;

	template <class ScannerType, class StorageType = build_static<1024, expand_strict>>
		class	channel_buffered_scanner : public channel_if
		{
			public:
				template< typename... Args >
					channel_buffered_scanner( feeder & afeed, Args&&... args)
						: formatter_(std::forward<Args>(args)...), channel_if( afeed ), buffer_<StorageType>() { }

				void	data( const buffer & adata ) ;

			protected:
				void	do_msg( void)
						{ pass( buffer_ ) ;  buffer_.reset() ;  formatter_.reset() ; }

				ScannerType formatter_ ;
				build_if	buffer_ ;
		} ;

	////
	// - details

	template <class ScanT, class StorT>
		void	channel_buffered_scanner<ScanT, StorT>::data( const buffer & adata )
		{
			static_assert( StorT::_traits::exceptions, "requires support for exceptions at buffer overflow" ) ;

			scanner<>	scanref( adata) ;
			size_t use_sz, fill_sz ;
			bool amsg ;
			int asignal = 0 ;

			while ( scanref && ! asignal )
			{
				try { amsg= formatter_.scan( buffer_, scanref ) ; }
					catch ( const std::bad_alloc& e ) { asignal = 1 ; }
					catch ( const std::length_error& e ) { asignal = 1 ; }

				if ( asignal ) { signal( asignal) ;  return ; }
				if ( amsg ) { do_msg() ; }
			}
		}

	static_assert( CtrlVec<'\0', '', '\a'>::mask == 0xf3, "") ;
} ;

#endif

