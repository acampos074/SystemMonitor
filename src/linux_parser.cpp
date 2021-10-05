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
  vector<string> jiffie_vector = LinuxParser::CpuUtilization(); 
  long totalJiffies = 0;
  // Ignore guest and guestnice since these are accounted 
  // in user and nice, respectively.
  int numCPUStates = 8;
  for(int i = 0;i < numCPUStates;i++){
    totalJiffies += std::stol(jiffie_vector[i]);
  }
  return totalJiffies;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  string value;
  long active_jiffies = 0;
  std::ifstream filestream(kProcDirectory+std::to_string(pid)+kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line);
    for(int i=0;i<23;i++){
      linestream >> value;
      //13,14,15,16
      // active jiffies = utime + stime + cutime + cstime
      if (i <= 16 && i >=13) {
        active_jiffies += stol(value);
        }
    }   
  }
  return active_jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffie_vector = LinuxParser::CpuUtilization(); 
  long activeJiffies = 0;
  // Ignore guest and guestnice since these are accounted 
  // in user and nice, respectively.
  int numCPUStates = 8;
  LinuxParser::CPUStates idle = LinuxParser::CPUStates::kIdle_;
  LinuxParser::CPUStates iowait = LinuxParser::CPUStates::kIOwait_;
  for(int i = 0;i < numCPUStates;i++){
    if(i != idle || i != iowait){
        activeJiffies += std::stol(jiffie_vector[i]);
    }
  }
  return activeJiffies; 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffie_vector = LinuxParser::CpuUtilization(); 
  long idleJiffies = 0;
  // Ignore guest and guestnice since these are accounted 
  // in user and nice, respectively.
  int numCPUStates = 8;
  LinuxParser::CPUStates idle = LinuxParser::CPUStates::kIdle_;
  LinuxParser::CPUStates iowait = LinuxParser::CPUStates::kIOwait_;
  for(int i = 0;i < numCPUStates;i++){
    if(i == idle || i == iowait){
        idleJiffies += std::stol(jiffie_vector[i]);
    }
  }
  return idleJiffies; 
 }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, jiffie;
  vector<string> jiffie_vector;
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
      jiffie_vector.push_back(jiffie);
    }
  }
  return jiffie_vector; 
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

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
    string command_line;
    std::ifstream filestream(kProcDirectory+std::to_string(pid)+kCmdlineFilename);
    if (filestream.is_open()) {
        std::getline(filestream, command_line);  
    }
  return command_line; 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  float ram_; 
  std::ifstream filestream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {ram_ = std::stof(value)*0.001;} // convert kB to MB
      }
    }
  }
  return std::to_string((int)ram_);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  
  string line, key, value, uid_;
  std::ifstream filestream(kProcDirectory+std::to_string(pid)+kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {uid_ = value;}
      }
    }
  }
  return uid_; 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  
  string line, key, value, user_, tmp;
  string uid_ = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user_ >> tmp >> value) {
        if (value == uid_) {
          return user_;
        }
      }
    }
  }
  return user_;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 

    string line,value;
    long uptime = 0;
    std::ifstream filestream(kProcDirectory+std::to_string(pid)+kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line); 
        std::istringstream linestream(line);
        for(int i=0;i<23;i++){
            linestream >> value;
            if (i == 21) {
                uptime = stol(value);
            }
        }   
    }
  
  return uptime; 
}
