#ifndef __BARRIER_H__
#define __BARRIER_H__

#include <mutex>
#include <semaphore.h>

class Barrier {
public:
  Barrier(int N);

  void wait(void);
  void post(void);

private:
  int N, n;
  std::mutex mtx;
  sem_t step[3];
};

#endif // __BARRIER_H__
