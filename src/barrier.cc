#include "barrier.h"

Barrier::Barrier(color equipo, int N) : equipo(equipo), N(N), n(0) {
  for (int i = 0; i < 3; i++) //A: Initialize all semaphores to not let anyone through
    sem_init(&step[i], 1, 0);
  sem_init(&step[3], 1, 1); //A: But do let the GameMaster through
  logMsg("[Barrier] DONE equipo=%i, N=%i\n", equipo, N);
}

color Barrier::wait(void) {
  sem_wait(&step[0]); //A: Wait until the operator lets us through
  logMsg("[Barrier/wait] step0 equipo=%i\n", equipo);

  mtx.lock();
  color _msg = msg;
  if (++n == N) {
    for (int i = 0; i < N; i++) sem_post(&step[1]);
    sem_post(&step[3]); //A: Let the GameMaster through for the next time
  }
  mtx.unlock();
  sem_wait(&step[1]); //A: Wait until everyone's here
  logMsg("[Barrier/wait] step1 equipo=%i\n", equipo);

  mtx.lock();
  if (--n == 0) for (int i = 0; i < N; i++) sem_post(&step[2]);
  mtx.unlock();
  sem_wait(&step[2]);
  logMsg("[Barrier/wait] step2 equipo=%i\n", equipo);

  return _msg;
}

void Barrier::post(color _msg) {
  sem_wait(&step[3]); //A: Make sure everyone's received the previous message
  logMsg("[Barrier/post] equipo=%i, msg=%i\n", equipo, _msg);

  msg = _msg;
  for (int i = 0; i < N; i++) //A: Let everyone through the first semaphore
    sem_post(&step[0]);
}
