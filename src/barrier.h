#ifndef __BARRIER_H__
#define __BARRIER_H__

#include <mutex>
#include <semaphore.h>

class Barrier {
public:
  Barrier(void);
  Barrier(int _N);

  int wait(void);
  void post(int _msg);

private:
  int N, n, msg;
  std::mutex mtx;
  sem_t step[3];
};

#endif // __BARRIER_H__
