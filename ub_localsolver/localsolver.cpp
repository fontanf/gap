#if LOCALSOLVER_FOUND

#include "gap/ub_localsolver/localsolver.hpp"

#include <localsolver.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <cmath>

using namespace gap;
using namespace localsolver;

class MyCallback: public LSCallback
{

public:

    MyCallback(Solution& sol, Info& info, std::vector<LSExpression>& agents):
        sol_(sol), info_(info), agents_(agents) { }

    void callback(LocalSolver& ls, LSCallbackType type) {
        LSSolutionStatus s = ls.getSolution().getStatus();
        if (s == SS_Infeasible)
            return;

        LSExpression obj = ls.getModel().getObjective(0);
        if(!sol_.feasible() || sol_.cost() > obj.getValue() + 0.5) {
            Solution sol_curr(sol_.instance());
            AgentIdx m = sol_.instance().agent_number();
            for (AgentIdx i=0; i<m; ++i) {
                LSCollection bin_collection = agents_[i].getCollectionValue();
                for (ItemIdx j_pos=0; j_pos<bin_collection.count(); ++j_pos) {
                    sol_curr.set(bin_collection[j_pos], i);
                }
            }
            std::stringstream ss;
            sol_.update(sol_curr, 0, ss, info_);
        }
    }

private:

    Solution& sol_;
    Info& info_;
    std::vector<LSExpression>& agents_;

};

Solution gap::ub_localsolver(LocalSolverData d)
{
    VER(d.info, "*** localsolver ***" << std::endl);
    ItemIdx n = d.ins.item_number();
    AgentIdx m = d.ins.agent_number();

    init_display(d.info);

    if (n == 0)
        return algorithm_end(d.sol, d.info);

    LocalSolver localsolver;

    // Remove display
    localsolver.getParam().setVerbosity(0);

    // Declares the optimization model.
    LSModel model = localsolver.getModel();

    // Weight of each item
    std::vector<std::vector<lsint>> item_weights(n, std::vector<lsint>(n));
    std::vector<std::vector<lsint>> item_costs(n, std::vector<lsint>(n));
    for (AgentIdx i=0; i<m; ++i) {
        for (ItemIdx j=0; j<n; ++j) {
            item_weights[i][j] = d.ins.alternative(j, i).w;
            item_costs[i][j] = d.ins.alternative(j, i).c;
        }
    }

    // Decision variables
    std::vector<LSExpression> agents(m);

    // Weight of each bin in the solution
    std::vector<LSExpression> agents_weight(m);

    // Profit of each bin in the solution
    std::vector<LSExpression> agents_cost(m);

    LSExpression total_cost; // Objective

    // Set decisions: bins[k] represents the items in bin k
    for (AgentIdx i=0; i<d.ins.agent_number(); ++i)
        agents[i] = model.setVar(d.ins.item_number());

    // Each item must be in one bin and one bin only
    model.constraint(model.partition(agents.begin(), agents.end()));

    // Create an array and a function to retrieve the item's weight
    std::vector<LSExpression> weight_array(m);
    std::vector<LSExpression> cost_array(m);
    std::vector<LSExpression> weight_selector(m);
    std::vector<LSExpression> cost_selector(m);
    for (AgentIdx i=0; i<m; ++i) {
        weight_array[i] = model.array(item_weights[i].begin(), item_weights[i].end());
        weight_selector[i] = model.createFunction([&](LSExpression j) {
                return weight_array[i][j]; });
        cost_array[i] = model.array(item_costs[i].begin(), item_costs[i].end());
        cost_selector[i] = model.createFunction([&](LSExpression j) {
                return cost_array[i][j]; });
    }

    for (AgentIdx i=0; i<m; ++i) {
        // Weight constraint for each bin
        agents_weight[i] = model.sum(agents[i], weight_selector[i]);
        model.constraint(agents_weight[i] <= (lsint)d.ins.capacity(i));
        agents_cost[i] = model.sum(agents[i], cost_selector[i]);
    }

    // Count the used bins
    total_cost = model.sum(agents_cost.begin(), agents_cost.end());

    // Minimize the number of used bins
    model.minimize(total_cost);

    model.close();

    // Time limit
    if (d.info.timelimit != std::numeric_limits<double>::infinity())
        localsolver.getParam().setTimeLimit(d.info.timelimit);

    // Custom callback
    MyCallback cb(d.sol, d.info, agents);
    localsolver.addCallback(CT_TimeTicked, &cb);

    // Solve
    localsolver.solve();

    // Retrieve solution
    for (AgentIdx i=0; i<m; ++i) {
        LSCollection bin_collection = agents[i].getCollectionValue();
        for (ItemIdx j_pos=0; j_pos<bin_collection.count(); ++j_pos)
            d.sol.set(bin_collection[j_pos], i);
    }

    return algorithm_end(d.sol, d.info);
}

#endif

