#include "gtest-1.8.1/gtest.h"
#include "testutils.h"

#include "gameMaster.h"
#include "config.h"
#include "utils.h"

#include <vector>
#include <semaphore.h>

void consumeBarrier(class Barrier *barrier) { //TODO: Definirla en testutils.cc me da error de compilación, no se da cuenta que ahora todo es público
  for (int i = 0; i < 3; i++)
    for (int n = 0; n < barrier->N; n++)
      sem_trywait(&barrier->step[i]);
}

TEST(GameMaster, init) {
  //******************************
  //S: Setup
  
  tablero_t tablero = fullSetup();

  //******************************
  //S: Test

  class Config config;
  class GameMaster belcebu(config);

  //A: Leyó las posiciones correctas
  EXPECT_EQ(belcebu.posiciones[ROJO], config.posiciones[ROJO]);
  EXPECT_EQ(belcebu.posiciones[AZUL], config.posiciones[AZUL]);

  //A: Tablero correcto
  EXPECT_EQ(belcebu.tablero, tablero);

  EXPECT_EQ(belcebu.currEquipo, EMPIEZA); //A: Empieza el equipo correcto
  EXPECT_EQ(belcebu.getGanador(), INDEFINIDO); //A: Empieza sin haber ganado alguien

  EXPECT_EQ(getSemValue(&belcebu.semBandera), 1); //A: Sólo deja que uno busque la bandera

  //A: Sólo dejó empezar al equipo que empieza
  EXPECT_EQ(getSemValue(&belcebu.barriers[EMPIEZA]->step[0]), config.cantJugadores);
  EXPECT_EQ(getSemValue(&belcebu.barriers[contrincante(EMPIEZA)]->step[0]), 0);
}

TEST(GameMaster, moverJugador) {
  //******************************
  //S: Setup

  tablero_t tablero = simpleSetup();
  
  //******************************
  //S: Test

  class Config config;
  class GameMaster belcebu(config);

  color equipo = belcebu.currEquipo; int nroJugador = 0; direccion dir = ARRIBA;
  struct Pos prevPos = belcebu.posiciones[equipo][nroJugador],
             newPos = prevPos.mover(dir);

  belcebu.moverJugador(dir, nroJugador);
  tablero[prevPos.y][prevPos.x] = VACIO;
  tablero[newPos.y][newPos.x] = equipo;

  //A: Actualizó la posición correcta
  EXPECT_EQ(belcebu.posiciones[equipo][nroJugador], newPos);

  //A: Actualizó al tablero correctamente
  EXPECT_EQ(belcebu.tablero, tablero);

  //A: Actualizó al ganador correctamente
  EXPECT_EQ(belcebu.getGanador(), equipo);

  //TODO: Movimiento no-ganador
  //TODO: Movimiento inválido
}

TEST(GameMaster, terminoRonda) {
  //******************************
  //S: Setup

  simpleSetup();

  //******************************
  //S: Test

  class Config config;
  class GameMaster belcebu(config);

  color equipo = belcebu.currEquipo;

  consumeBarrier(belcebu.barriers[equipo]);
  belcebu.terminoRonda(equipo);

  //A: Le toca al nuevo equipo
  EXPECT_EQ(belcebu.currEquipo, contrincante(equipo));

  //A: Deja pasar al equipo correcto
  EXPECT_EQ(getSemValue(&belcebu.barriers[equipo]->step[0]), 0);
  EXPECT_EQ(getSemValue(&belcebu.barriers[contrincante(equipo)]->step[0]), config.cantJugadores);

  consumeBarrier(belcebu.barriers[contrincante(equipo)]);
  belcebu.terminoRonda(contrincante(equipo));

  //A: Le vuelve a tocar al primer equipo
  EXPECT_EQ(belcebu.currEquipo, equipo);

  //A: Nuevamente deja pasar al equipo correcto
  EXPECT_EQ(getSemValue(&belcebu.barriers[equipo]->step[0]), config.cantJugadores);
  EXPECT_EQ(getSemValue(&belcebu.barriers[contrincante(equipo)]->step[0]), 0);

  belcebu.moverJugador(ARRIBA, 0); //NOTA: Movida ganadora
  consumeBarrier(belcebu.barriers[equipo]);
  belcebu.terminoRonda(equipo);

  //A: Deja pasar a ambos equipos
  EXPECT_EQ(getSemValue(&belcebu.barriers[equipo]->step[0]), config.cantJugadores);
  EXPECT_EQ(getSemValue(&belcebu.barriers[contrincante(equipo)]->step[0]), config.cantJugadores);
}

TEST(GameMaster, enPosicion) {
  //******************************
  //S: Setup

  tablero_t tablero = simpleSetup();

  //******************************
  //S: Test

  class Config config;
  class GameMaster belcebu(config);

  //A: Todas las posiciones valen lo mismo
  for (int y = 0; y < tablero.size(); y++)
    for (int x = 0; x < tablero[0].size(); x++)
      EXPECT_EQ(belcebu.enPosicion({x, y}), tablero[y][x]);
}

TEST(GameMaster, mePuedoMover) {
  //******************************
  //S: Setup

  int ancho = 3, alto = 3, cantJugadores = 2;
  struct Pos banderas[2] = {{2, 2}, {0, 0}};
  std::vector<struct Pos> posiciones[2] = {
    {{0, 2}, {1, 1}},
    {{1, 0}, {1, 2}}
  };
  createConfig(ancho, alto, cantJugadores, banderas, posiciones);

  /*
  tablero = {
    {BANDERA_ROJA, ROJO , VACIO},
    {VACIO, AZUL, VACIO},
    {AZUL, ROJO, BANDERA_AZUL}
  };
  */

  //******************************
  //S: Test

  class Config config;
  class GameMaster belcebu(config);

  //NOTA: currEquipo empieza siendo ROJO
  struct Pos r1 = config.posiciones[ROJO][0], r2 = config.posiciones[ROJO][1]; //U: Posición de los jugadores 

  EXPECT_FALSE(belcebu.mePuedoMover(r1, ARRIBA)); //A: Me salgo del mapa
  EXPECT_FALSE(belcebu.mePuedoMover(r1, ABAJO)); //A: Me muevo sobre un jugador
  EXPECT_FALSE(belcebu.mePuedoMover(r1, IZQUIERDA)); //A: Me muevo sobre mi bandera

  EXPECT_TRUE(belcebu.mePuedoMover(r1, DERECHA)); //A: A VACIO
  EXPECT_TRUE(belcebu.mePuedoMover(r2, DERECHA)); //A: Sobre bandera enemiga
}

TEST(GameMaster, esPosicionValida) {
  //******************************
  //S: Setup

  tablero_t tablero = simpleSetup();

  //******************************
  //S: Test

  class Config config;
  class GameMaster belcebu(config);

  //A: Todo el tablero es válido
  for (int y = 0; y < tablero.size(); y++)
    for (int x = 0; x < tablero[0].size(); x++)
      EXPECT_TRUE(belcebu.esPosicionValida({x, y}));

  //A: Afuera en y es inválido
  for (int x = 0; x < tablero[0].size(); x++) {
    EXPECT_FALSE(belcebu.esPosicionValida({x, -1}));
    EXPECT_FALSE(belcebu.esPosicionValida({x, (int)tablero.size()})); //NOTA: Casteo a int porque me tira un error de compilación por tablero_t
  }

  //A: Afuera en x es inválido
  for (int y = 0; y < tablero.size(); y++) {
    EXPECT_FALSE(belcebu.esPosicionValida({-1, -1}));
    EXPECT_FALSE(belcebu.esPosicionValida({(int)tablero[0].size(), y})); //NOTA: Casteo a int porque me tira un error de compilación por tablero_t
  }
}

TEST(GameMaster, isEmpty) {
  //******************************
  //S: Setup

  fullSetup();

  //******************************
  //S: Test

  class Config config;
  class GameMaster belcebu(config);

  //NOTA: Ver el tablero de fullSetup 
  EXPECT_FALSE(belcebu.isEmpty({0, 0}));
  EXPECT_FALSE(belcebu.isEmpty({1, 0}));

  EXPECT_TRUE(belcebu.isEmpty({1, 1}));
  EXPECT_TRUE(belcebu.isEmpty({2, 1}));

  EXPECT_FALSE(belcebu.isEmpty({1, 2}));
}

TEST(GameMaster, hasFlag) {
  //******************************
  //S: Setup

  fullSetup();

  //******************************
  //S: Test

  class Config config;
  class GameMaster belcebu(config);

  //NOTA: Ver el tablero de fullSetup 
  //A: Encuentra las banderas
  EXPECT_TRUE(belcebu.hasFlag({0, 0}, ROJO));
  EXPECT_TRUE(belcebu.hasFlag({2, 2}, AZUL));

  //A: Todo lo otro no es ninguna bandera
  for (color equipo : {ROJO, AZUL}) {
    EXPECT_FALSE(belcebu.hasFlag({1, 0}, equipo));
    EXPECT_FALSE(belcebu.hasFlag({1, 1}, equipo));
    EXPECT_FALSE(belcebu.hasFlag({1, 2}, equipo));
  }
}
