#pragma once

#if KNITRO_FOUND

#include "generalizedassignmentsolver/solution.hpp"

namespace generalizedassignmentsolver
{

struct MilpKnitroOptionalParameters
{
    optimizationtools::Info info = optimizationtools::Info();

    const Solution* initial_solution = NULL;
    bool only_linear_relaxation = false;
};

struct MilpKnitroOutput: Output
{
    MilpKnitroOutput(
            const Instance& instance,
            optimizationtools::Info& info):
        Output(instance, info) { }

    MilpKnitroOutput& algorithm_end(
            optimizationtools::Info& info);

    std::vector<std::vector<double>> x;
};

MilpKnitroOutput milp_knitro(
        const Instance& instance,
        MilpKnitroOptionalParameters parameters = {});

}

#endif
