#ifndef __BARRIER_H__
#define __BARRIER_H__

#include "utils.h"

#include <mutex>
#include <semaphore.h>

class Barrier {
public:
  Barrier(color equipo, int N);

  color wait(void);
  void post(color _msg);

#ifndef TEST
private:
#endif // TEST
  int N, n; color equipo, msg;
  std::mutex mtx;
  sem_t step[4];
};

#endif // __BARRIER_H__
