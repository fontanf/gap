#pragma once

#include "gap/lib/instance.hpp"
#include "gap/lib/solution.hpp"

namespace gap
{

Cost lb_colgen_dip(const Instance& ins, Info info = Info());
Cost lb_lagrelax_assignment_dip(const Instance& ins, Info info = Info());

Solution sopt_branchandcut_dip(const Instance& ins, Info info = Info());
Solution sopt_priceandcut_dip(const Instance& ins, Info info = Info());
Solution sopt_relaxandcut_dip(const Instance& ins, Info info = Info());

}

