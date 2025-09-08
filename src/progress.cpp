#include "progress.h"

ProgressTracker::ProgressTracker(size_t total_work, std::string name) 
    : total(total_work), task_name(std::move(name)) {
    start_time = std::chrono::steady_clock::now();
}

void ProgressTracker::update(size_t completed) {
    current += completed;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
    
    if (elapsed > 0 && current > 0) {
        double progress = static_cast<double>(current) / total;
        double rate = static_cast<double>(current) / elapsed;
        double eta = (total - current) / rate;
        
        std::cout << "\r" << task_name << ": " 
                 << std::fixed << std::setprecision(1) << (progress * 100) << "% "
                 << "(" << current << "/" << total << ") "
                 << "Rate: " << std::setprecision(0) << rate << "/s "
                 << "ETA: " << static_cast<int>(eta) << "s     " << std::flush;
    }
}

void ProgressTracker::finish() {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start_time).count();
    std::cout << "\r" << task_name << ": COMPLETED in " 
             << elapsed << "ms (" << current << " items)\n";
}
