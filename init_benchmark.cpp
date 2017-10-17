#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/hpx_start.hpp>
#include <hpx/exception.hpp>
#include <hpx/throw_exception.hpp>
#include <hpx/parallel/algorithms/sort.hpp>
#include <hpx/parallel/algorithms/minmax.hpp>
#include <hpx/util/high_resolution_clock.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

static int argc_;
static char ** argv_;
static std::vector<double> timings_init;
static std::vector<double> timings_finalize;
static hpx::util::high_resolution_timer timer_init;
static hpx::util::high_resolution_timer timer_finalize;

int hpx_main(int argc, char ** argv)
{
    timings_init.push_back(timer_init.elapsed());
    timer_finalize.restart();
    return hpx::finalize();
}

void benchmark_init_finalize_main()
{
    timer_init.restart();
    hpx::init(argc_, argv_);
    timings_finalize.push_back(timer_finalize.elapsed());
}

void benchmark_init_finalize_cb()
{
    timer_init.restart();
    hpx::init(hpx_main, argc_, argv_);
    timings_finalize.push_back(timer_finalize.elapsed());
}

void benchmark_start_stop_main()
{
    timer_init.restart();
    hpx::start(argc_, argv_);
    hpx::stop();
    timings_finalize.push_back(timer_finalize.elapsed());
}

void benchmark_start_stop_cb()
{
    timer_init.restart();
    hpx::start(hpx_main, argc_, argv_);
    hpx::stop();
    timings_finalize.push_back(timer_finalize.elapsed());
}

template <typename F>
void benchmark_one(F benchmark_func, const int n)
{
    timings_init.clear();
    timings_finalize.clear();

    for (int i = 0; i < n; ++i)
    {
        benchmark_func();
    }

    auto init_min = std::min_element(std::begin(timings_init), std::end(timings_init));
    auto finalize_min = std::min_element(std::begin(timings_finalize), std::end(timings_finalize));
    auto init_max = std::max_element(std::begin(timings_init), std::end(timings_init));
    auto finalize_max = std::max_element(std::begin(timings_finalize), std::end(timings_finalize));
    auto init_avg = std::accumulate(std::begin(timings_init), std::end(timings_init), 0.0) / n;
    auto finalize_avg = std::accumulate(std::begin(timings_finalize), std::end(timings_finalize), 0.0) / n;

    std::cout << *init_min * 1e6 << ", "
              << *init_max * 1e6 << ", "
              << init_avg * 1e6 << ", "
              << *finalize_min * 1e6 << ", "
              << *finalize_max * 1e6 << ", "
              << finalize_avg * 1e6 << "\n";
}

int main(int argc, char ** argv)
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("full", "print full results (default: false)")
        ;

    argc_ = argc;
    argv_ = argv;

    // TODO: Call hpx::init with different arguments.
    // TODO: Call with commandline arguments?
    // TODO: Modify resource partitioner?
    // TODO: Overhead in e.g. OpenMP?

    std::cout << "test, init min [µs], init max [µs], init avg [µs], finalize min [µs], finalize max [µs], finalize avg [µs]\n";

    std::cout << "init/finalize/cb, ";
    benchmark_one(&benchmark_init_finalize_cb, 50);

    std::cout << "init/finalize/main, ";
    benchmark_one(&benchmark_init_finalize_main, 50);

    std::cout << "start/stop/cb, ";
    benchmark_one(&benchmark_start_stop_cb, 50);

    std::cout << "start/stop/main, ";
    benchmark_one(&benchmark_start_stop_main, 50);
}

