#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/exception.hpp>
#include <hpx/throw_exception.hpp>
#include <hpx/parallel/algorithms/sort.hpp>
#include <hpx/parallel/algorithms/minmax.hpp>
#include <hpx/util/high_resolution_clock.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

int hpx_future_then_timing(int n, int argc, char ** argv)
{
    hpx::lcos::local::promise<int> a_promise;
    hpx::future<int> a = a_promise.get_future();

    for (int i = 0; i < n; ++i)
    {
        a = a.then([](hpx::future<int>) { return 0; });
    }

    std::uint64_t time = hpx::util::high_resolution_clock::now();

    a_promise.set_value(0);
    a.get();

    time = hpx::util::high_resolution_clock::now() - time;

    std::cout << "Chained n = " << n << " futures\n";
    std::cout << "Execution took " << (time * 1e-9) << " s (" << (time / n) << " ns/future)\n";

    return hpx::finalize();
}

int hpx_init_finalize_time(
    std::vector<double>& init_timings,
    hpx::util::high_resolution_timer& timer_init,
    hpx::util::high_resolution_timer& timer_finalize,
    int argc,
    char ** argv)
{
    init_timings.push_back(timer_init.elapsed());
    timer_finalize = hpx::util::high_resolution_timer();
    return hpx::finalize();
}

int main(int argc, char ** argv)
{
    using hpx::util::placeholders::_1;
    using hpx::util::placeholders::_2;

    int n = 100000;

    hpx::util::function_nonser<int(int, char**)> callback =
        hpx::util::bind(&hpx_future_then_timing, n, _1, _2);

    hpx::init(callback, argc, argv);

    std::vector<double> init_timings, finalize_timings;

    // TODO: Call hpx::init with different arguments.
    // TODO: Call with commandline arguments?
    // TODO: Modify resource partitioner?
    // TODO: Overhead in e.g. OpenMP?
    for (int i = 0; i < 1; ++i)
    {
        hpx::util::high_resolution_timer timer_init, timer_finalize;

        hpx::util::function_nonser<int(int, char**)> callback_init_finalize =
            hpx::util::bind(&hpx_init_finalize_time, std::ref(init_timings), std::ref(timer_init), std::ref(timer_finalize), _1, _2);

        timer_init = hpx::util::high_resolution_timer();
        hpx::init(callback_init_finalize, argc, argv);

        finalize_timings.push_back(timer_finalize.elapsed());
        //std::cout << init_timings[i] << ", " << finalize_timings[i] << "\n";
    }

    std::cout << "init [s], finalize [s]\n";
    auto init_min = std::min_element(std::begin(init_timings), std::end(init_timings));
    auto finalize_min = std::min_element(std::begin(finalize_timings), std::end(finalize_timings));
    auto init_max = std::max_element(std::begin(init_timings), std::end(init_timings));
    auto finalize_max = std::max_element(std::begin(finalize_timings), std::end(finalize_timings));

    std::cout << "min:\n";
    std::cout << *init_min << ", " << *finalize_min << "\n";
    std::cout << "max:\n";
    std::cout << *init_max << ", " << *finalize_max << "\n";
}

