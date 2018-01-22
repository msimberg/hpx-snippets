#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/hpx_start.hpp>
#include <hpx/exception.hpp>
#include <hpx/throw_exception.hpp>
#include <hpx/parallel/algorithms/sort.hpp>
#include <hpx/parallel/algorithms/minmax.hpp>
#include <hpx/util/high_resolution_clock.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

static std::vector<double> timings_start;
static std::vector<double> timings_stop;
static std::vector<double> timings_main;
static std::vector<double> timings_finalize;
static hpx::util::high_resolution_timer timer_start;
static hpx::util::high_resolution_timer timer_stop;
static hpx::util::high_resolution_timer timer_main;
static hpx::util::high_resolution_timer timer_finalize;

int hpx_main(int, char**)
{
    timings_main.push_back(timer_main.elapsed());
    timer_finalize.restart();
    return hpx::finalize();
}

struct statistics
{
    double min;
    double max;
    double average;
};

struct statistics compute_statistics(std::vector<double> x)
{
    struct statistics result;

    result.min = *std::min_element(std::begin(x), std::end(x));
    result.max = *std::max_element(std::begin(x), std::end(x));
    result.average = std::accumulate(std::begin(x), std::end(x), 0.0) / x.size();

    return result;
}

int main(int argc, char ** argv)
{
    const int n = 10;

    timings_start.reserve(n);
    timings_stop.reserve(n);
    timings_main.reserve(n);
    timings_finalize.reserve(n);

    // std::cout
    //     << "threads, "
    //     << "first start [s], first stop [s], first main [s], first finalize [s], "
    //     << "start min [s], start max [s], start avg [s], "
    //     << "stop min [s], stop max [s], stop avg [s], "
    //     << "main min [s], main max [s], main avg [s], "
    //     << "finalize min [s], finalize max [s], finalize avg [s]"
    //     << "\n";

    // Time first run
    timer_start.restart();
    timer_main.restart();

    hpx::start(1, nullptr);

    timings_start.push_back(timer_start.elapsed());
    timer_stop.restart();

    hpx::stop(hpx::runtime_exit_mode_shutdown);

    timings_finalize.push_back(timer_finalize.elapsed());
    timings_stop.push_back(timer_stop.elapsed());

    std::cout
        << timings_start[0] << ", "
        << timings_stop[0] << ", "
        << timings_main[0] << ", "
        << timings_finalize[0] << ", ";

    // Reset results
    timings_start.clear();
    timings_stop.clear();
    timings_main.clear();
    timings_finalize.clear();

    // Time multiple runs
    for (int i = 0; i < n; ++i)
    {
        timer_start.restart();
        timer_main.restart();

        hpx::start(1, nullptr);

        timings_start.push_back(timer_start.elapsed());
        timer_stop.restart();

        hpx::stop(hpx::runtime_exit_mode_shutdown);

        timings_finalize.push_back(timer_finalize.elapsed());
        timings_stop.push_back(timer_stop.elapsed());
    }

    auto start_results = compute_statistics(timings_start);
    auto stop_results = compute_statistics(timings_stop);
    auto main_results = compute_statistics(timings_main);
    auto finalize_results = compute_statistics(timings_finalize);

    std::cout
        << start_results.min << ", " << start_results.max << ", " << start_results.average << ", "
        << stop_results.min << ", " << stop_results.max << ", " << stop_results.average << ", "
        << main_results.min << ", " << main_results.max << ", " << main_results.average << ", "
        << finalize_results.min << ", " << finalize_results.max << ", " << finalize_results.average
        << "\n";

    hpx::start(1, nullptr);
    hpx::stop(hpx::runtime_exit_mode_shutdown);
}

