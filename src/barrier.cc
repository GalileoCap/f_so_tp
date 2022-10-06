#include "barrier.h"

Barrier::Barrier(void) {} //U: Default constructor without initializing semaphores

Barrier::Barrier(int N) : N(N), n(0) {
  for (int i = 0; i < 3; i++) //A: Initialize all semaphores to not let anyone through
    sem_init(&step[i], 1, 0);
}

int Barrier::wait(void) {
  sem_wait(&step[0]); //A: Wait until the operator lets us through

  mtx.lock();
  int res = msg;

  if (++n == N) for (int i = 0; i < N; i++) sem_post(&step[1]);
  mtx.unlock();
  sem_wait(&step[1]); //A: Wait until everyone's here

  mtx.lock();
  if (--n == 0) for (int i = 0; i < N; i++) sem_post(&step[2]);
  mtx.unlock();
  sem_wait(&step[2]);

  return res;
}

void Barrier::post(int _msg) {
  msg = _msg;
  for (int i = 0; i < N; i++) //A: Let everyone through the first semaphore
    sem_post(&step[0]);
}
