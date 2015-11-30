#ifndef __HWInterface_Timer_h
#define __HWInterface_Timer_h

#include <iostream>
#include <chrono>
#include <ctime>

using namespace std::chrono;
class Timer {

public:
  Timer() : start_(), end_() {}
  virtual ~Timer() {}
  void start() {
    start_ = system_clock::now();
  }
  void stop() {
    end_ = system_clock::now();
  }
  void show(const std::string& label) {
    duration<double> time_span = duration_cast<duration<double>>(end_ - start_);
    std::time_t end_time = system_clock::to_time_t(end_);

    const std::string& tnow = std::ctime(&end_time); 
    std::cout << label
              << " finished at: " << tnow
              << "\telapsed time: " << time_span.count() << " seconds" << std::endl;
  }
  void reset() {start_ = end_;}

private:
  system_clock::time_point start_, end_;
};
#endif
