// SearchWorker.cpp

#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <chrono>
#include <iostream>
#include <utility>
#include <atomic>
#include "SearchWorker.hpp"

SearchWorker::SearchWorker() : thread_(&SearchWorker::threadLoop, this) {} // starts the worker thread

    SearchWorker::~SearchWorker()
    { // It tells the worker to stop, then waits for it to finish.
        {
            std::lock_guard<std::mutex> lock(mutex_); //lock to write
            stop_ = true; // writes
        }
        cv_.notify_one(); // Setting stop_ = true only changes a variable, and a sleeping thread doesn't notice. Without the notify it never wakes, so join() waits forever for a thread that is waiting to be woken.
        thread_.join();
    }

void SearchWorker::submit(Request x)
{ // the GUI puts a job in the mailbox and wakes the worker.
    {
        std::lock_guard<std::mutex> lock(mutex_);//lock to write
        pending_ = std::move(x); // writes
    }
    cv_.notify_one();
}

//the GUI asks “is there an answer yet?” It returns immediately, either with the answer or with nothing
std::optional<Response> SearchWorker::poll()
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::optional<Response> r = finished_; // reads the mailbox
    finished_.reset(); // empties the mailbox
    return r;
}

void SearchWorker::threadLoop()
{ // the worker's whole life. Sleep until there's a job, do it, leave the answer in the other mailbox, repeat.
    while (true)
    {
        std::cout << "worker: going to sleep\n";
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return pending_.has_value() || stop_; }); // wait until there's a job or stop is requested
        std::cout << "worker: woke up\n";
        if (stop_)
            return;

        Request job = *pending_; // get the job out of the mailbox
        pending_.reset();        // clear mailbox
        lock.unlock();           // slow part runs WITHOUT the lock
        
        std::this_thread::sleep_for(std::chrono::seconds(THREAD_SLEEP_SECONDS));
        
        auto start = std::chrono::steady_clock::now();
        SearchResult result = runSearch(job.grid, job.start, job.end, job.algo);
        double ms = std::chrono::duration<double, std::milli>(
                        std::chrono::steady_clock::now() - start).count();
        
        Response answer{result, ms, job.version};

        lock.lock();
        finished_ = answer;
    }
}

// Thread: a second worker running a function at the same time as your main program.
// Join: “wait here until that worker is finished.”
// Mutex: a key. Only one thread can hold it, so only one touches the shared data at a time.
// Condition variable: lets a thread sleep until someone wakes it, instead of checking over and over.
