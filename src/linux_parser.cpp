#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key, value;
  float mem_total, mem_free; 
  std::ifstream filestream(kProcDirectory+kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {mem_total = std::stof(value);}
        else if (key == "MemFree:") {mem_free = std::stof(value);}
      }
    }
  }
  return (mem_total-mem_free)/(mem_total);
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, value;
  long uptime_ = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    uptime_ = std::stol(value);
  }
  return uptime_;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string jiffie, line;
  long totalJiffies = 0;
  // Ignore guest and guestnice since these are accounted 
  // in user and nice, respectively.
  int numCPUStates = 8; 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> jiffie; // to take into account cpu
    for(int i = 0;i < numCPUStates;i++){
      linestream >> jiffie;
      totalJiffies += std::stol(jiffie);
    }
  }
  return totalJiffies; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string jiffie, line;
  long activeJiffies = 0;
  // Ignore guest and guestnice since these are accounted 
  // in user and nice, respectively.
  int numCPUStates = 8;
  LinuxParser::CPUStates idle = LinuxParser::CPUStates::kIdle_;
  LinuxParser::CPUStates iowait = LinuxParser::CPUStates::kIOwait_;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> jiffie; // to take into account cpu
    for(int i = 0;i < numCPUStates;i++){
      linestream >> jiffie;
      if(i != idle || i != iowait){
        activeJiffies += std::stol(jiffie);
      }
    }
  }
  return activeJiffies; 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string jiffie, line;
  long idleJiffies = 0;
  // Ignore guest and guestnice since these are accounted 
  // in user and nice, respectively.
  int numCPUStates = 8;
  LinuxParser::CPUStates idle = LinuxParser::CPUStates::kIdle_;
  LinuxParser::CPUStates iowait = LinuxParser::CPUStates::kIOwait_;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> jiffie; // to take into account cpu
    for(int i = 0;i < numCPUStates;i++){
      linestream >> jiffie;
      if(i == idle || i == iowait){
        idleJiffies += std::stol(jiffie);
      }
    }
  }
  return idleJiffies; 
 }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  return {};
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {return value;}
      }
    }
  }
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses(){ 
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {return value;}
      }
    }
  }
  return value;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
