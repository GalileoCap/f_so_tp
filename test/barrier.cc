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
  EXPECT_EQ(getSemValue(&barrier.step[0]), 0);
  EXPECT_EQ(getSemValue(&barrier.step[1]), 0);
  EXPECT_EQ(getSemValue(&barrier.step[2]), 0);
  EXPECT_EQ(getSemValue(&barrier.step[3]), 1);
}

TEST(Barrier, post) {
  color equipo = ROJO; int N = 3;
  class Barrier barrier(equipo, N);

  color msg = ROJO;
  barrier.post(msg);

  //A: Mandó el mensaje correcto
  EXPECT_EQ(barrier.msg, msg);

  //A: Deja pasar a N sólo en el primer step
  EXPECT_EQ(getSemValue(&barrier.step[0]), N);
  EXPECT_EQ(getSemValue(&barrier.step[1]), 0);
  EXPECT_EQ(getSemValue(&barrier.step[2]), 0);

  //A: No deja hast post hasta ser consumido
  EXPECT_EQ(getSemValue(&barrier.step[3]), 0);

  //A: Deja hacer post luego de ser consumido
  barrier.consume();
  EXPECT_EQ(getSemValue(&barrier.step[3]), 1);
}

//TODO: Wait
