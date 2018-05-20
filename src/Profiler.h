/*********************************************************************
 * This file is distributed as part of the C++ port of the APRIL tags
 * library. The code is licensed under GPLv2.
 *
 * Original author: Edwin Olson <ebolson@umich.edu>
 * C++ port and modifications: Matt Zucker <mzucker1@swarthmore.edu>
 ********************************************************************/

#ifndef _PROFILER_H_
#define _PROFILER_H_

#include <map>
#include <chrono>

// Lightweight classes/structs used for profiling AprilTags.

//////////////////////////////////////////////////////////////////////
// Encapsulate a step/substep of an algorithm.

class AlgorithmStep {
public:

  int step;
  int substep;

  AlgorithmStep(): step(0), substep(0) {}
  AlgorithmStep(int v): step(v), substep(0) {}
  AlgorithmStep(int v, int i): step(v), substep(i) {}

};

// We need to compare and sort these. 

inline bool operator==(const AlgorithmStep& s1, const AlgorithmStep& s2) {
  return s1.step == s2.step && s1.substep == s2.substep;
}

inline bool operator<(const AlgorithmStep& s1, const AlgorithmStep& s2) {
  return ( (s1.step < s2.step) ||
           (s1.step == s2.step && s1.substep < s2.substep) );
}

//////////////////////////////////////////////////////////////////////
// Structure used for profiling.

struct TimingInfo {
  
  double start;
  double run;
  const char* desc;

  TimingInfo(): run(0) {}
  
};

typedef std::map<AlgorithmStep, TimingInfo> TimingLookup;

//////////////////////////////////////////////////////////////////////
// Class for profiling



class Profiler {
	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::microseconds Ms;

private:
	std::chrono::time_point<Time, Ms> init;

public:
  
  TimingLookup timers;
  
  int num_iterations;
  int num_detections;

  Profiler(): num_iterations(0), num_detections(0) {}


  double getTimeAsDouble() {
	  auto now = std::chrono::time_point_cast<Ms>(Time::now());
	  return (now - init).count();
  }

  void start(int step, int substep, const char* desc) {

    init = std::chrono::time_point_cast<Ms>(Time::now());

    std::pair<TimingLookup::iterator, bool> info =
      timers.insert(std::make_pair(AlgorithmStep(step, substep), TimingInfo()));

    TimingLookup::iterator i = info.first;

    i->second.start = getTimeAsDouble();
    i->second.desc = desc;
  }

  void end(int step, int substep) { 
    TimingLookup::iterator i = timers.find(AlgorithmStep(step, substep));
    i->second.run += getTimeAsDouble() - i->second.start;
    if (i->first == AlgorithmStep(0,0)) { ++num_iterations; }
  }

};

#endif
