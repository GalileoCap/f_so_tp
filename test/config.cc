#include "gtest-1.8.1/gtest.h"
#include "testutils.h"

#include "config.h"

TEST(Config, completo) {
  //******************************
  //S: Setup

  int ancho = 5, alto = 5, cantJugadores = 2;
  struct Pos banderas[2] = {{0, 0}, {4, 4}};
  std::vector<struct Pos> posiciones[2] = {
    {{1, 1}, {1, 2}},
    {{3, 3}, {3, 2}}
  };
  createConfig(ancho, alto, cantJugadores, banderas, posiciones);

  //******************************
  //S: Test

  struct Config config;

  ASSERT_EQ(config.ancho, ancho);
  ASSERT_EQ(config.alto, alto);
  ASSERT_EQ(config.cantJugadores, cantJugadores);
  for (color equipo : {ROJO, AZUL}) {
    ASSERT_EQ(config.banderas[equipo], banderas[equipo]);
    ASSERT_EQ(config.posiciones[equipo], posiciones[equipo]);
  }
}

