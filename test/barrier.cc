#include "gtest-1.8.1/gtest.h"
#include "testutils.h"

#include "barrier.h"

#include <semaphore.h>

TEST(Barrier, init) {
  int N = 3;
  class Barrier barrier(N);

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
  int N = 3;
  class Barrier barrier(N);

  //A: Después de un post deja pasar a N sólo en el primer step
  barrier.post();
  int sval; sem_getvalue(&barrier.step[0], &sval); //TODO: getvalue error
  EXPECT_EQ(sval, N);
  sem_getvalue(&barrier.step[1], &sval); //TODO: getvalue error
  EXPECT_EQ(sval, 0);
  sem_getvalue(&barrier.step[2], &sval); //TODO: getvalue error
  EXPECT_EQ(sval, 0);
}

//TODO: Wait
