#include "gap/lib/generator.hpp"

#include <random>

using namespace gap;

std::ostream& gap::operator<<(std::ostream& os, const Generator& data)
{
    os << "n " << data.n
        << " m " << data.m
        << " t " << data.t
        << " r " << data.r
        << " x " << data.x
        << " s " << data.s
        ;
    return os;
}

Instance Generator::generate()
{
    g.seed(s);
    std::normal_distribution<double> d_n(n, n / 10);
    std::normal_distribution<double> d_m(m, m / 10);
    ItemIdx  n_eff = std::round(d_n(g));
    AgentIdx m_eff = std::round(d_m(g));

    Instance ins(m_eff);
    std::normal_distribution<double> d_wj(r / 2, r / 10);
    Weight wsum_min = 0;
    Weight wsum_max = 0;
    for (ItemIdx j=0; j<n_eff; ++j) {
        ins.add_item();
        Weight wj = d_wj(g);
        Weight wj_min = r;
        Weight wj_max = 0;
        std::normal_distribution<double> d_wij(wj, wj / 10);
        for (AgentIdx i=0; i<m_eff; ++i) {
            Weight wij;
            do {
                wij = std::round(d_wij(g));
            } while (wij <= 0 || wij > r);
            std::normal_distribution<double> d_cij(r - wij, (r - wij) / 10);
            Cost cij;
            do {
                cij = std::round(d_cij(g));
            } while (cij <= 0 || cij > r);
            ins.set_alternative(j, i, wij, cij);
            if (wj_max < wij)
                wj_max = wij;
            if (wj_min > wij)
                wj_min = wij;
        }
        wsum_min += wj_min;
        wsum_max += wj_max;
    }
    double c = (double)(wsum_min) * (1 - x) + (double)wsum_max * x;
    c = c / m_eff;
    for (AgentIdx i=0; i<m_eff; ++i)
        ins.set_capacity(i, std::ceil(c));
    return ins;
}

