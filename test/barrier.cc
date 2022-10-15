#include "gtest-1.8.1/gtest.h"
#include "testutils.h"

#include "barrier.h"
#include "utils.h"

#include <semaphore.h>

TEST(Barrier, init) {
  color equipo = ROJO; int N = 3;
  class Barrier barrier(equipo, N);

  EXPECT_EQ(barrier.equipo, equipo);
  EXPECT_EQ(barrier.N, N);

  //A: Empieza sin dejar pasar a nadie en ningún step
  int sval; sem_getvalue(&barrier.step[0], &sval); //TODO: getvalue error
  EXPECT_EQ(sval, 0);
  sem_getvalue(&barrier.step[1], &sval); //TODO: getvalue error
  EXPECT_EQ(sval, 0);
  sem_getvalue(&barrier.step[2], &sval); //TODO: getvalue error
  EXPECT_EQ(sval, 0);
}

TEST(Barrier, post) {
  color equipo = ROJO; int N = 3;
  class Barrier barrier(equipo, N);

  color msg = ROJO;
  barrier.post(msg);

  //A: Mandó el mensaje correcto
  EXPECT_EQ(barrier.msg, msg);

  //A: Deja pasar a N sólo en el primer step
  int sval; sem_getvalue(&barrier.step[0], &sval); //TODO: getvalue error
  EXPECT_EQ(sval, N);
  sem_getvalue(&barrier.step[1], &sval); //TODO: getvalue error
  EXPECT_EQ(sval, 0);
  sem_getvalue(&barrier.step[2], &sval); //TODO: getvalue error
  EXPECT_EQ(sval, 0);
}

//TODO: Wait
