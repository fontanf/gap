#include "generalizedassignmentsolver/algorithms/branchandprice.hpp"

#include "generalizedassignmentsolver/algorithms/columngeneration.hpp"
#include "generalizedassignmentsolver/algorithms/greedy.hpp"

#include <queue>
#include <set>

using namespace generalizedassignmentsolver;

BranchAndPriceOutput& BranchAndPriceOutput::algorithm_end(Info& info)
{
    PUT(info, "Algorithm", "NodeNumber", node_number);
    Output::algorithm_end(info);
    VER(info, "Nodes: " << node_number << std::endl);
    return *this;
}

struct BranchAndPriceNode
{
    std::shared_ptr<BranchAndPriceNode> father = nullptr;
    AltIdx k = -1;
    int8_t v = 0;
    double value = 0;
};

BranchAndPriceOutput generalizedassignmentsolver::branchandprice(
        const Instance& instance, BranchAndPriceOptionalParameters parameters)
{
    VER(parameters.info, "*** branchandprice"
            << " --lp-solver " << parameters.lp_solver
            << " --tree-search-algorithm " << parameters.tree_search_algorithm
            << " --branching-rule " << parameters.branching_rule
            << " ***" << std::endl);
    BranchAndPriceOutput output(instance, parameters.info);

    // Initialize column generation parameters.
    std::vector<std::vector<std::vector<ItemIdx>>> columns(instance.agent_number());
    std::vector<int> fixed_alternatives(instance.alternative_number(), -1);
    ColGenOptionalParameters colgen_parameters;
    colgen_parameters.columns   = &columns;
    colgen_parameters.fixed_alt = &fixed_alternatives;
    colgen_parameters.lp_solver = parameters.lp_solver;

    // Nodes
    auto comp = [](
            const std::shared_ptr<BranchAndPriceNode>& node_1,
            const std::shared_ptr<BranchAndPriceNode>& node_2) {
        return node_1->value < node_2->value; };
    std::multiset<std::shared_ptr<BranchAndPriceNode>, decltype(comp)> nodes(comp);

    // Root node.
    auto root = std::make_shared<BranchAndPriceNode>();
    colgen_parameters.info.set_timelimit(parameters.info.remaining_time());
    auto colgen_output = columngeneration(instance, colgen_parameters);
    std::stringstream ss;
    ss << "root node";
    output.update_lower_bound(colgen_output.lower_bound, ss, parameters.info);
    nodes.insert(root);

    while (!nodes.empty()) {
        output.node_number++;

        // Check time
        if (!parameters.info.check_time())
            return output.algorithm_end(parameters.info);

        // Get node
        auto node = *nodes.begin();
        nodes.erase(nodes.begin());

        // Initialize solution and fixed_alternatives
        Solution solution(instance);
        std::fill(fixed_alternatives.begin(), fixed_alternatives.end(), -1);
        auto node_tmp = node;
        AltIdx depth = 0;
        while (node_tmp->father != nullptr) {
            if (node_tmp->v == 1) {
                auto a = instance.alternative(node_tmp->k);
                solution.set(node_tmp->k);
                for (AgentIdx i = 0; i < instance.agent_number(); ++i)
                    fixed_alternatives[instance.alternative_index(a.j, i)] = 0;
            }
            fixed_alternatives[node_tmp->k] = node_tmp->v;
            node_tmp = node_tmp->father;
            depth++;
        }

        // Run column generation
        auto colgen_output = columngeneration(instance, colgen_parameters);
        if (parameters.tree_search_algorithm == "bfs") {
            std::stringstream ss;
            ss << "node " << output.node_number;
            output.update_lower_bound(colgen_output.lower_bound, ss, parameters.info);
        }
        //std::cout << "node_number " << output.node_number
            //<< " n " << solution.item_number()
            //<< " c " << solution.cost()
            //<< " lb " << colgen_output.lower_bound
            //<< std::endl;

        // Check bound
        if (output.solution.feasible()
                && output.solution.cost() <= colgen_output.lower_bound)
            continue;

        // Compute next variable to branch on.
        AltIdx k_best = -1;
        AgentIdx i_best = -1;
        if (parameters.branching_rule == "most-fractional") {
            for (ItemIdx j = 0; j < instance.item_number(); ++j) {
                for (AgentIdx i = 0; i < instance.agent_number(); ++i) {
                    AltIdx k = instance.alternative_index(j, i);
                    if (fixed_alternatives[k] != -1)
                        continue;
                    if (k_best == -1
                            || std::abs(colgen_output.x[k_best] - 0.5)
                            < std::abs(colgen_output.x[k] - 0.5)) {
                        k_best = k;
                        i_best = i;
                    }
                }
            }
        } else {
            for (ItemIdx j = 0; j < instance.item_number(); ++j) {
                for (AgentIdx i = 0; i < instance.agent_number(); ++i) {
                    AltIdx k = instance.alternative_index(j, i);
                    if (fixed_alternatives[k] != -1)
                        continue;
                    if (k_best == -1
                            || colgen_output.x[k_best] < colgen_output.x[k]) {
                        k_best = k;
                        i_best = i;
                    }
                }
            }
        }

        if (solution.remaining_capacity(i_best) < instance.alternative(k_best).w)
            continue;

        // Update solution
        solution.set(k_best);
        if (solution.feasible()) {
            std::stringstream ss;
            ss << "node " << output.node_number;
            output.update_solution(solution, ss, parameters.info);
            if (output.optimal())
                return output.algorithm_end(parameters.info);
            continue;
        }

        // Child 1
        auto child_1 = std::make_shared<BranchAndPriceNode>();
        child_1->father = node;
        child_1->k = k_best;
        child_1->v = 1;
        if (parameters.tree_search_algorithm == "dfs") {
            child_1->value = - 0.5 - depth;
        } else if (parameters.tree_search_algorithm == "lds") {
            child_1->value = node->value;
        } else {
            fixed_alternatives[k_best] = 1;
            auto colgen_output_child = columngeneration(instance, colgen_parameters);
            child_1->value = (double)colgen_output_child.lower_bound - (double)(solution.item_number() + 1) / instance.item_number();
        }
        nodes.insert(child_1);

        // Child 0
        auto child_0 = std::make_shared<BranchAndPriceNode>();
        child_0->father = node;
        child_0->k = k_best;
        child_0->v = 0;
        if (parameters.tree_search_algorithm == "dfs") {
            child_0->value = - depth;
        } else if (parameters.tree_search_algorithm == "lds") {
            child_0->value = node->value + colgen_output.x[k_best];
        } else {
            fixed_alternatives[k_best] = 0;
            auto colgen_output_child = columngeneration(instance, colgen_parameters);
            child_0->value = (double)colgen_output_child.lower_bound - (double)solution.item_number() / instance.item_number();
        }
        nodes.insert(child_0);
    }

    return output.algorithm_end(parameters.info);
}

