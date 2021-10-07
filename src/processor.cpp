#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
double Processor::Utilization() {
    static long prev_idle = 0;
    static long prev_total = 0;
    Processor::idle_ = LinuxParser::IdleJiffies();
    Processor::total_ = LinuxParser::Jiffies();
    Processor::cpu_utilization_ = (double)((Processor::total_-prev_total)-(Processor::idle_-prev_idle))/(double)(Processor::total_-prev_total);
    prev_idle = Processor::idle_;
    prev_total = Processor::total_;

    return Processor::cpu_utilization_; 
}