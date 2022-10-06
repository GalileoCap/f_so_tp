#include "barrier.h"

Barrier::Barrier(int N) {
  this->N = N;
  this->n = 0;

  for (int i = 0; i < 3; i++)
    sem_init(&this->step[i], 1, 0);
}

void Barrier::wait() {
  sem_wait(&this->step[0]); //A: Waits until the operator opens it

  mtx.lock();
  if (++n == N) for (int i = 0; i < N; i++) sem_post(&this->step[1]);
  mtx.unlock();
  sem_wait(&this->step[1]);

  mtx.lock();
  if (--n == 0) for (int i = 0; i < N; i++) sem_post(&this->step[2]);
  mtx.unlock();
  sem_wait(&this->step[2]);
}

void Barrier::post() {
  for (int i = 0; i < N; i++)
    sem_post(&this->step[0]);
}
