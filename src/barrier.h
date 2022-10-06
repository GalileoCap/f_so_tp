#ifndef __BARRIER_H__
#define __BARRIER_H__

#include <mutex>
#include <semaphore.h>

struct Barrier {
  Barrier();
  Barrier(int N);

  void wait();
  void post();

  int N, n;
  std::mutex mtx;
  sem_t step[3];
};

#endif // __BARRIER_H__
