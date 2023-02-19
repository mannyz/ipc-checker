#include "run/as/common/scheduler.hpp"

#include <stdexcept>

#include <sched.h>
#include <unistd.h>

void SchedSetAffinity(int cpu_id) {
  constexpr int DEFAULT_CPU = -1;
  if (cpu_id == DEFAULT_CPU) {
    return;
  }

#if !defined(__APPLE__)
  cpu_set_t cpu_set;
  CPU_ZERO(&cpu_set);
  CPU_SET(cpu_id, &cpu_set);

  if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpu_set) == -1) {
    throw std::runtime_error("sched_setaffinity() failed");
  }
#endif
}