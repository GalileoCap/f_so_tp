#include "gtest-1.8.1/gtest.h"
#include "testutils.h"

#include "gameMaster.h"
#include "equipo.h"
#include "config.h"
#include "utils.h"

TEST(Equipo, init) {
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

  color c = ROJO;
  class Equipo equipo(&belcebu, c, strat, config.cantJugadores, quantum, config.posiciones[c]);

  EXPECT_EQ(equipo.belcebu, &belcebu);
  EXPECT_EQ(equipo.equipo, c);
  EXPECT_EQ(equipo.strat, strat);
  EXPECT_EQ(equipo.posiciones, config.posiciones[c]);
  EXPECT_EQ(equipo.quantum, quantum);

  //NOTA: El init de las estrategias lo revisa cada una
}

//TODO: moverse

TEST(Equipo, buscarBanderas) {
  //******************************
  //S: Setup

  fullSetup();

  //******************************
  //S: Test

  const estrategia strat = SECUENCIAL;
  const int quantum = 3;

  class Config config;
  class GameMaster belcebu(config);

  class Equipo equipo(&belcebu, ROJO, strat, config.cantJugadores, quantum, config.posiciones[ROJO]);
  equipo.buscarBanderas();
  
  EXPECT_EQ(equipo.banderas[contrincante(equipo.equipo)], config.banderas[contrincante(equipo.equipo)]);
}

TEST(Equipo, buscarSecuencial) {
  //******************************
  //S: Setup

  fullSetup();

  //******************************
  //S: Test

  const estrategia strat = SECUENCIAL;
  const int quantum = 3;

  class Config config;
  class GameMaster belcebu(config);

  class Equipo equipo(&belcebu, ROJO, strat, config.cantJugadores, quantum, config.posiciones[ROJO]);
  equipo.buscarSecuencial();
  
  EXPECT_EQ(equipo.banderas[contrincante(equipo.equipo)], config.banderas[contrincante(equipo.equipo)]);
}

//TODO: buscarThread
//TODO: secuencial
//TODO: roundrobin
//TODO: shortestdistancefirst
//TODO: nuestra
