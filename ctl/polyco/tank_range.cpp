#include <polyco>

using ctl::__detail::tank_range_b ;

void	tank_range_b::step() noexcept
{
	if ( ptr_ && ( ptr_ < limit_ ))
	{
		tank_ctrl_base * rec= (tank_ctrl_base *) ptr_ ;
		ptr_ += rec-> size_ ;
	}
}

