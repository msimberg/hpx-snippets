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

int main()
{
    int x = 0;
    int n = 10000;

    {
        hpx::util::high_resolution_timer timer;

#       pragma omp parallel
        {
            x += 3;
        }

        auto t = timer.elapsed();

        // std::cout << "first loop: " << t * 1e6 << " µs\n\n";
    }

    // std::cout << "test, min [µs], max [µs], avg [µs]\n";

    {
        std::vector<double> timings;
        hpx::util::high_resolution_timer timer;

        for (int i = 0; i < n; ++i)
        {
            timer.restart();

#           pragma omp parallel
            {
                x += 3;
            }

            timings.push_back(timer.elapsed());
        }

        auto timings_min = std::min_element(std::begin(timings), std::end(timings));
        auto timings_max = std::max_element(std::begin(timings), std::end(timings));
        auto timings_avg = std::accumulate(std::begin(timings), std::end(timings), 0.0) / n;

        std::cout
            << *timings_min << ", "
            << *timings_max << ", "
            << timings_avg << ", "
            << x << "\n";
    }

    {
        std::vector<double> timings;
        hpx::util::high_resolution_timer timer;

        for (int i = 0; i < n; ++i)
        {
            timer.restart();
            x += 3;
            timings.push_back(timer.elapsed());
        }

        auto timings_min = std::min_element(std::begin(timings), std::end(timings));
        auto timings_max = std::max_element(std::begin(timings), std::end(timings));
        auto timings_avg = std::accumulate(std::begin(timings), std::end(timings), 0.0) / n;

        // std::cout << "timing_overhead" << ", "
        //         << *timings_min * 1e6 << ", "
        //         << *timings_max * 1e6 << ", "
        //         << timings_avg * 1e6 << "\n";
    }

    return x;
}
