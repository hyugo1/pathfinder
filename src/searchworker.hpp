// SearchWorker.hpp

#pragma once

#include "grid.hpp"
#include "algorithms.hpp"

#include <optional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

inline std::atomic<int> THREAD_SLEEP_SECONDS {0};//safe to read and write from several threads at once, with no lock needed

struct Request
{
    Grid grid;
    Position start;
    Position end;
    Algorithm algo;
    int version;
};

struct Response
{
    SearchResult result;
    double executionTimeMs;
    int version;
};

class SearchWorker
{
public:
    SearchWorker(); // starts the worker thread
    ~SearchWorker(); // It tells the worker to stop, then waits for it to finish.

    void submit(Request x); // the GUI puts a job in the mailbox and wakes the worker.
    std::optional<Response> poll(); //the GUI asks “is there an answer yet?”

private: 
    void threadLoop(); // the worker's whole life. Sleep until there's a job, do it, leave the answer in the other mailbox, repeat.

    std::mutex mutex_;
    std::condition_variable cv_;
    std::optional<Request> pending_; // mailbox for incoming jobs
    std::optional<Response> finished_; // mailbox for outgoing results
    bool stop_ = false; // flag to signal the worker to stop
    std::thread thread_;
};