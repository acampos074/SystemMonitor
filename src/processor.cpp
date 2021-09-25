#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    static long prevIdle = 0;
    static long prevTotal = 0;
    Processor::idle_ = LinuxParser::IdleJiffies();
    Processor::total_ = LinuxParser::Jiffies();
    Processor::cpu_utilization_ = (float)((Processor::total_-prevTotal)-(Processor::idle_-prevIdle))/(float)(Processor::total_-prevTotal);
    prevIdle = Processor::idle_;
    prevTotal = Processor::total_;

    return Processor::cpu_utilization_; 
}