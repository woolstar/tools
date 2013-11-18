// format channel -*- C++ -*-

#ifndef _BTL_FMTCHAN_H
#define _BTL_FMTCHAN_H 1

namespace btl
{
	class	channelf_text : public channel_if
	{
		public:
			channelf_text( feeder & ) ;

			void	data( const buffer & ) ;
	} ;

	class	channelf_packet : public channel_if
	{
		public:
			channelf_text( feeder & ) ;

			void	data( const buffer & ) ;
	} ;

} ;

#endif

