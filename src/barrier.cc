#include "barrier.h"

Barrier::Barrier(color equipo, int N) : equipo(equipo), N(N), n(0) {
  for (int i = 0; i < 3; i++) //A: Initialize all semaphores to not let anyone through
    sem_init(&step[i], 1, 0);
  logMsg("BARRIER done equipo=%i, N=%i\n", equipo, N);
}

color Barrier::wait(void) {
  sem_wait(&step[0]); //A: Wait until the operator lets us through
  logMsg("BARRIER step0 equipo=%i\n", equipo);

  mtx.lock();
  if (++n == N) for (int i = 0; i < N; i++) sem_post(&step[1]);
  mtx.unlock();
  sem_wait(&step[1]); //A: Wait until everyone's here
  logMsg("BARRIER step1 equipo=%i\n", equipo);

  mtx.lock();
  if (--n == 0) for (int i = 0; i < N; i++) sem_post(&step[2]);
  mtx.unlock();
  sem_wait(&step[2]);
  logMsg("BARRIER step2 equipo=%i\n", equipo);

  return msg;
}

void Barrier::post(color _msg) {
  logMsg("BARRIER post equipo=%i, msg=%i\n", equipo, _msg);

  msg = _msg;
  for (int i = 0; i < N; i++) //A: Let everyone through the first semaphore
    sem_post(&step[0]);
}
