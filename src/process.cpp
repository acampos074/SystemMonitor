#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"
#include "system.h"
#include <unistd.h>

using std::string;
using std::to_string;
using std::vector;

// Initializes the private members of the Process class

Process::Process(int pid)
{
    pid_ = pid;
    
    user_ = LinuxParser::User(pid_);
    ram_ = LinuxParser::Ram(pid_);
    command_ = LinuxParser::Command(pid_);
    uptime_ = LinuxParser::UpTime(pid_);
    cpuUtilization_ = Process::CpuUtilization();
}


// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 

    // start time in clock ticks
    string line,value;
    long start_time = 0;
    std::ifstream filestream(LinuxParser::kProcDirectory+std::to_string(pid_)+LinuxParser::kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line); 
        std::istringstream linestream(line);
        for(int i=0;i<23;i++){
            linestream >> value;
            // active jiffies = utime + stime + cutime + cstime
            if (i == 21) {
                start_time = stol(value);
            }
        }   
    }
    // system uptime in seconds
    long uptime = LinuxParser::UpTime();
    // [14,15,16,17,22] = [utime,stime,cutime,cstime,starttime];
    long sys_clock_hertz = sysconf(_SC_CLK_TCK);
    float seconds = uptime - ((float)start_time/(float)sys_clock_hertz);
    //long total_time = utime + stime + cutime + cstime;
    long total_time = LinuxParser::ActiveJiffies(pid_);
    float cpu_utilization = (((float)total_time/(float)sys_clock_hertz)/seconds);
    return cpu_utilization;
}

// Return the command that generated this process
string Process::Command() { return command_; }

// Return this process's memory utilization
string Process::Ram() { return ram_; }

// Return the user (name) that generated this process
string Process::User() { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    
    long sys_clock_hertz = sysconf(_SC_CLK_TCK);
    float uptime_seconds = (float)uptime_/(float)sys_clock_hertz;
    return (long int)uptime_seconds; 
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process & a) { return a.CpuUtilization() < this->CpuUtilization(); }