#pragma once
#include <atomic>
#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>

class ProgressTracker {
    std::atomic<size_t> current{0};
    size_t total;
    std::chrono::steady_clock::time_point start_time;
    std::string task_name;
    
public:
    ProgressTracker(size_t total_work, std::string name);
    void update(size_t completed = 1);
    void finish();
};
