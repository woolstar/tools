// iter_implement.h -*- C++ -*-

#ifndef _CTL_ITERIMPL
#define _CTL_ITERIMPL	1

namespace ctl
{
	template <typename IterA, typename IterB, typename Container>
		inline bool
			operator==(
				const iterator<IterA, Container> & lit,
				const iterator<IterB, Container> & rit
			)
			{ return lit.base() == rit.base() ; }

	template <typename IterA, typename IterB, typename Container>
		inline bool
			operator!=(
				const iterator<IterA, Container> & lit,
				const iterator<IterB, Container> & rit
			)
			{ return lit.base() != rit.base() ; }

	template <typename IterA, typename IterB, typename Container>
		inline auto
			operator-(
				const iterator<IterA, Container> & lit,
				const iterator<IterB, Container> & rit
			)
			-> decltype( rit.base() - lit.base() ) { return rit.base() - lit.base() ; }
} ;

#endif

