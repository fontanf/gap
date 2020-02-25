#if GECODE_FOUND

#include "generalizedassignmentsolver/algorithms/constraintprogramming_gecode.hpp"

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>
#include <gecode/gist.hh>

using namespace generalizedassignmentsolver;
using namespace Gecode;

/**
 * Useful links
 * https://www.gecode.org/doc-latest/MPG.pdf
 */

ConstraintProgrammingGecodeOutput& ConstraintProgrammingGecodeOutput::algorithm_end(Info& info)
{
    Output::algorithm_end(info);
    return *this;
}

class GapGecode: public IntMinimizeSpace
{

public:

    GapGecode(const Instance& ins, ConstraintProgrammingGecodeOptionalParameters& p):
        ins_(ins),
        p_(p),
        xij0_(*this, ins.item_number() * ins.agent_number(), 0, 1),
        xj_(*this, ins.item_number(), 0, ins.agent_number() - 1),
        cj_(*this, ins.item_number()),
        load_(*this, ins.agent_number())
    {
        ItemIdx n = ins.item_number();
        AgentIdx m = ins.agent_number();

        // Channel xij == 1 <=> xj == i
        Matrix<BoolVarArgs> xij(xij0_, n, m);
        for (ItemIdx j=0; j<n; j++)
            channel(*this, xij.col(j), xj_[j]);
        for (ItemIdx j=0; j<n; j++)
            rel(*this, sum(xij.col(j)) == 1);

        // Weights
        weights_ = std::vector<IntArgs>(m, IntArgs(n));
        for (AgentIdx i=0; i<m; ++i)
            for (ItemIdx j=0; j<n; ++j)
                weights_[i][j] = ins.alternative(j, i).w;

        // Costs
        costs_ = std::vector<IntArgs>(n, IntArgs(m));
        for (ItemIdx j=0; j<n; ++j)
            for (AgentIdx i=0; i<m; ++i)
                costs_[j][i] = ins.alternative(j, i).c;

        // Cost variables
        for (ItemIdx j=0; j<n; j++) {
            cj_[j] = IntVar(*this, ins.item(j).c_min, ins.item(j).c_max);
            element(*this, costs_[j], xj_[j], cj_[j]);
        }
        c_ = expr(*this, sum(cj_));

        // Load variables (and capacity constraints)
        for (AgentIdx i=0; i<m; ++i) {
            load_[i] = IntVar(*this, 0, ins.capacity(i));
            linear(*this, weights_[i], xij.row(i), IRT_EQ, load_[i]);
        }

        // Branch on the most efficient agent
        auto v = [](const Space& home, IntVar x, int j)
        {
            const Instance& ins = static_cast<const GapGecode&>(home).instance();
            AgentIdx i_best = -1;
            double c_best = -1;
            for (IntVarValues i(x); i(); ++i) {
                AltIdx k = ins.alternative_index(j, i.val());
                double c = ins.profit(k) / ins.alternative(k).w;
                if (i_best == -1 || c_best < c) {
                    i_best = i.val();
                    c_best = c;
                }
            }
            return i_best;
        };
        branch(*this, xj_, INT_VAR_NONE(), INT_VAL(v));
    }

    ~GapGecode() {  }

    GapGecode(GapGecode& s):
        IntMinimizeSpace(s), ins_(s.ins_), p_(s.p_), weights_(s.weights_), costs_(s.costs_)
    {
        xij0_.update(*this, s.xij0_);
        xj_.update(*this, s.xj_);
        cj_.update(*this, s.cj_);
        c_.update(*this, s.c_);
        load_.update(*this, s.load_);
    }

    virtual IntMinimizeSpace* copy(void) { return new GapGecode(*this); }

    virtual IntVar cost(void) const { return c_; }

    void print(void) const
    {
        std::cout << "Cost " << c_ << std::endl;
    }

    AgentIdx agent(ItemIdx j) const { return xj_[j].val(); }
    const Instance& instance() const { return ins_; }

private:

    const Instance& ins_;
    ConstraintProgrammingGecodeOptionalParameters& p_;
    std::vector<IntArgs> weights_;
    std::vector<IntArgs> costs_;
    BoolVarArray xij0_;
    IntVarArray xj_;
    IntVarArray cj_;
    IntVar c_;
    IntVarArray load_;

};

ConstraintProgrammingGecodeOutput generalizedassignmentsolver::constraintprogramming_gecode(const Instance& ins, ConstraintProgrammingGecodeOptionalParameters p)
{
    VER(p.info, "*** constraintprogramming_gecode ***" << std::endl);
    ConstraintProgrammingGecodeOutput output(ins, p.info);

    if (ins.item_number() == 0)
        return output.algorithm_end(p.info);

    GapGecode model(ins, p);
    //Gist::bab(&model);

    Search::Options options;

    // Time limit
    if (p.info.timelimit != std::numeric_limits<double>::infinity()) {
        Search::Stop* stoptime = Search::Stop::time(p.info.timelimit * 1000);
        options.stop = stoptime;
    }

    BAB<GapGecode> engine(&model, options);
    GapGecode* sol_ptr = NULL;
    Solution sol_best(ins);
    while ((sol_ptr = engine.next())) {
        Solution sol_curr(ins);
        for (ItemIdx j=0; j<ins.item_number(); j++)
            sol_curr.set(j, sol_ptr->agent(j));
        output.update_solution(sol_curr, std::stringstream(""), p.info);
        delete sol_ptr;
    }

    if (p.info.check_time()) {
        Cost lb = (output.solution.feasible())? output.solution.cost(): ins.bound();
        output.update_lower_bound(lb, std::stringstream(""), p.info);
    }

    return output.algorithm_end(p.info);
}

#endif
