#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/hpx_start.hpp>
#include <hpx/include/resource_partitioner.hpp>
#include <hpx/include/threads.hpp>
#include <hpx/exception.hpp>
#include <hpx/runtime/threads/policies/scheduler_mode.hpp>
#include <hpx/runtime/threads/policies/schedulers.hpp>
#include <hpx/throw_exception.hpp>
#include <hpx/parallel/algorithms/sort.hpp>
#include <hpx/parallel/algorithms/minmax.hpp>
#include <hpx/util/high_resolution_clock.hpp>

#include <boost/program_options.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <tuple>
#include <vector>

static std::vector<double> timings_start;
static std::vector<double> timings_async;
static std::vector<double> timings_main;
static std::vector<double> timings_stop;
static hpx::util::high_resolution_timer timer;

int hpx_main(boost::program_options::variables_map& vm)
{
    timings_main.push_back(timer.elapsed());
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
    boost::program_options::options_description desc_commandline;
    desc_commandline.add_options()
        ("delay-exit", boost::program_options::value<bool>()->default_value(true), "Delay exit of scheduling loops")
        ("background-work", boost::program_options::value<bool>()->default_value(true), "Do background work")
        ("repetitions", boost::program_options::value<std::uint64_t>()->default_value(100), "Number of repetitions")
        ;

    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::command_line_parser(argc, argv)
        .allow_unregistered()
        .options(desc_commandline)
        .run(),
        vm);

    bool delay_exit = vm["delay-exit"].as<bool>();
    bool background_work = vm["background-work"].as<bool>();
    std::uint64_t repetitions = vm["repetitions"].as<std::uint64_t>();

    timings_start.reserve(repetitions);
    timings_main.reserve(repetitions);
    timings_stop.reserve(repetitions);

    hpx::start(argc, argv);
    std::uint64_t threads = hpx::resource::get_num_threads("default");
    hpx::stop();

    // Time multiple runs
    for (int i = 0; i < repetitions; ++i)
    {
        timer.restart();

        hpx::start(desc_commandline, argc, argv);

        timings_start.push_back(timer.elapsed());
        timings_async.push_back(timer.elapsed());

        hpx::stop();
        timings_stop.push_back(timer.elapsed());

        std::cout
            << threads << ", "
            << std::fixed
            << timings_start[i] << ", "
            << timings_async[i] << ", "
            << timings_main[i] << ", "
            << timings_stop[i]
            << "\n";
    }

    // auto start_results = compute_statistics(timings_start);
    // auto async_results = compute_statistics(timings_async);
    // auto main_results = compute_statistics(timings_main);
    // auto stop_results = compute_statistics(timings_stop);

    // std::cout
    //     << std::fixed
    //     << start_results.min << ", " << start_results.max << ", " << start_results.average << ", "
    //     << async_results.min << ", " << async_results.max << ", " << async_results.average << ", "
    //     << main_results.min << ", " << main_results.max << ", " << main_results.average << ", "
    //     << stop_results.min << ", " << stop_results.max << ", " << stop_results.average
    //     << "\n";
}

