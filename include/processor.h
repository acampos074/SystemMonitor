#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  double Utilization();  // See src/processor.cpp

 private:
    long total_;
    long idle_;
    double cpu_utilization_;
};

#endif