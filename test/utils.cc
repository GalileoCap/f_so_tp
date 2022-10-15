#include "gtest-1.8.1/gtest.h"
#include "testutils.h"

#include "utils.h"

TEST(Utils, posInit) {
  int x = 0, y = 0;
  struct Pos pos({x, y});

  EXPECT_EQ(pos.x, x);
  EXPECT_EQ(pos.y, y);
}

TEST(Utils, posMover) {
  struct Pos pos({0, 0});

  EXPECT_EQ(pos.mover(IZQUIERDA), Pos({-1, 0}));
  EXPECT_EQ(pos.mover(DERECHA), Pos({1, 0}));
  EXPECT_EQ(pos.mover(ARRIBA), Pos({0, 1}));
  EXPECT_EQ(pos.mover(ABAJO), Pos({0, -1}));
}

TEST(Utils, posEq) {
  struct Pos pos({0, 0});

  EXPECT_TRUE(pos == Pos({0, 0}));
  EXPECT_FALSE(pos == Pos({1, 0}));
  EXPECT_FALSE(pos == Pos({0, 1}));
  EXPECT_FALSE(pos == Pos({1, 1}));
}

TEST(Utils, contrincante) {
  EXPECT_EQ(contrincante(ROJO), AZUL);
  EXPECT_EQ(contrincante(AZUL), ROJO);
}

TEST(Utils, bandera) {
  EXPECT_EQ(bandera(ROJO), BANDERA_ROJA);
  EXPECT_EQ(bandera(AZUL), BANDERA_AZUL);
}
