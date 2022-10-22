#include "gtest-1.8.1/gtest.h"
#include "testutils.h"

#include "gameMaster.h"
#include "equipo.h"
#include "config.h"
#include "utils.h"

TEST(Ejemplo, completo) {
  //******************************
  //S: Setup

  int ancho = 5, alto = 5, cantJugadores = 1;
  struct Pos banderas[2] = {{4, 4},{0, 0}};
  std::vector<struct Pos> posiciones[2] = {
    {{3, 3}},
    {{1, 1}}
  };
  createConfig(ancho, alto, cantJugadores, banderas, posiciones);

  //******************************
  //S: Test

  const estrategia strat = SECUENCIAL;
  const int quantum = 3;

  class Config config;
  class GameMaster belcebu(config);

  class Equipo rojo(&belcebu, ROJO, strat, config.cantJugadores, quantum, config.posiciones[ROJO]),
               azul(&belcebu, AZUL, strat, config.cantJugadores, quantum, config.posiciones[AZUL]);

  rojo.comenzar();
  azul.comenzar();
  rojo.terminar();
  azul.terminar();

  EXPECT_EQ(belcebu.ganador, ROJO);
}
