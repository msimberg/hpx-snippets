#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <hwloc.h>

enum class thread_state
{
    running,
    stopped,
    suspending,
    suspended
};

static hwloc_topology_t topo;
static std::mutex topo_mtx;

void thread_func(std::atomic<thread_state>& state, std::mutex& mtx, std::condition_variable& cond, int thread_num)
{
    {
        std::lock_guard<std::mutex> lk(topo_mtx);

        hwloc_cpuset_t cpuset = hwloc_bitmap_alloc();
        hwloc_bitmap_only(cpuset, thread_num);

        if (!hwloc_set_cpubind(topo, cpuset, HWLOC_CPUBIND_STRICT | HWLOC_CPUBIND_THREAD))
            std::terminate();

        hwloc_bitmap_free(cpuset);
    }

    while (state != thread_state::stopped)
    {
        if (state == thread_state::suspending)
        {
            // std::cout << "suspending in " << thread_num << std::endl;
            state = thread_state::suspended;
            std::unique_lock<std::mutex> lk(mtx);
            cond.wait(lk);
            state = thread_state::running;
        }
    }
}

void suspend_all(std::vector<std::atomic<thread_state>>& states)
{
    for (auto& s : states)
    {
        s.store(thread_state::suspending);
    }

    for (auto const& s : states)
    {
        while (s.load() == thread_state::suspending)
        {
            std::this_thread::yield();
        }
    }
}

void resume_all(std::vector<std::condition_variable>& conds, std::vector<std::atomic<thread_state>>& states)
{
    for (auto& c : conds)
    {
        c.notify_all();
    }

    for (int i = 0; i < conds.size(); ++i)
    {
        while (states[i].load() != thread_state::running)
        {
            conds[i].notify_all();
            std::this_thread::yield();
        }
    }
}

void start_all(std::vector<std::atomic<thread_state>>& states,
               std::vector<std::thread>& threads,
               std::vector<std::mutex>& mtxs,
               std::vector<std::condition_variable>& conds)
{
    for (int i = 0; i < states.size(); ++i)
    {
        states[i].store(thread_state::running);
        threads[i] = std::thread(thread_func, std::ref(states[i]), std::ref(mtxs[i]), std::ref(conds[i]), i);
    }
}

void stop_all(std::vector<std::atomic<thread_state>>& states, std::vector<std::thread>& threads)
{
    for (std::size_t i = 0; i < states.size(); ++i)
    {
        states[i].store(thread_state::stopped);
    }

    for (std::size_t i = 0; i < states.size(); ++i)
    {
        threads[i].join();
    }
}


int main(int, char ** argv)
{
    const int num_threads = std::stoi(argv[1]);
    const int repetitions = std::stoi(argv[2]);

    hwloc_topology_init(&topo);

    std::vector<std::atomic<thread_state>> states(num_threads);
    std::vector<std::condition_variable> conds(num_threads);
    std::vector<std::mutex> mtxs(num_threads);
    std::vector<std::thread> threads(num_threads);

    start_all(states, threads, mtxs, conds);

    suspend_all(states);

    for (int i = 0; i < repetitions; ++i)
    {
        auto start = std::chrono::high_resolution_clock::now();

        resume_all(conds, states);

        auto tmp = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> resume = tmp - start;

        suspend_all(states);

        tmp = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> suspend = tmp - start;

        std::cout << num_threads << ", " << resume.count() << ", " << suspend.count() << std::endl;
    }

    resume_all(conds, states);

    stop_all(states, threads);

    hwloc_topology_destroy(topo);
}
