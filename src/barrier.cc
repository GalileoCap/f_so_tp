#include "barrier.h"
#include "utils.h"

Barrier::Barrier(int N) : N(N), n(0) {
  for (int i = 0; i < 3; i++) //A: Initialize all semaphores to not let anyone through
    sem_init(&step[i], 1, 0);
  //logMsg("BARRIER done N=%i\n", N);
}

void Barrier::wait(void) {
  //logMsg("BARRIER wait\n");
  sem_wait(&step[0]); //A: Wait until the operator lets us through
  //logMsg("BARRIER step0\n");

  mtx.lock();
  if (++n == N) for (int i = 0; i < N; i++) sem_post(&step[1]);
  mtx.unlock();
  sem_wait(&step[1]); //A: Wait until everyone's here
  //logMsg("BARRIER step1\n");

  mtx.lock();
  if (--n == 0) for (int i = 0; i < N; i++) sem_post(&step[2]);
  mtx.unlock();
  sem_wait(&step[2]);
  //logMsg("BARRIER step2\n");
}

void Barrier::post(void) {
  //logMsg("BARRIER post\n");
  for (int i = 0; i < N; i++) //A: Let everyone through the first semaphore
    sem_post(&step[0]);
}
