#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // See src/processor.cpp

 private:
    long total_;
    long idle_;
    float cpu_utilization_;
};

#endif