#pragma once

#include "gap/lib/instance.hpp"
#include "gap/lib/solution.hpp"

namespace gap
{

struct DualLSShiftSwapData
{
    const Instance& ins;
    std::default_random_engine& gen;
    Info info = Info();
};
Solution sol_dualls_shiftswap(DualLSShiftSwapData d);

struct LSFirstShiftSwapData
{
    const Instance& ins;
    std::default_random_engine& gen;
    double alpha;
    Info info = Info();
};
Solution sol_lsfirst_shiftswap(LSFirstShiftSwapData d);

struct LSBestShiftSwapData
{
    const Instance& ins;
    std::default_random_engine& gen;
    double alpha;
    Info info = Info();
};
Solution sol_lsbest_shiftswap(LSBestShiftSwapData d);

struct TSShiftSwapData
{
    const Instance& ins;
    std::default_random_engine& gen;
    double alpha;
    Info info = Info();
};
Solution sol_ts_shiftswap(TSShiftSwapData d);

struct SAShiftSwapData
{
    const Instance& ins;
    std::default_random_engine& gen;
    double alpha;
    Info info = Info();
    double beta = 0.99;
    Cpt l = 100000;
};
Solution sol_sa_shiftswap(SAShiftSwapData d);

struct PRShiftSwapData
{
    const Instance& ins;
    std::default_random_engine& gen;
    Info info = Info();
    std::vector<double> alpha;
    Cpt rho = 20;
    Cpt gamma = 10;
    double delta_inc = 0.01;
    double delta_dec = 0.1;
};
Solution sol_pr_shiftswap(PRShiftSwapData d);

bool doubleshift_best(const Instance& ins, Solution& sol, Info& info);
bool tripleswap_best(const Instance& ins, Solution& sol, Info& info);

}

