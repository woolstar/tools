#include <polyco>

using ctl::__detail::tank_iter_b ;

void	tank_iter_b::step() noexcept
{
	tank_ctrl_base * rec= (tank_ctrl_base *) ptr_ ;
	ptr_ += rec-> size_ ;
}

