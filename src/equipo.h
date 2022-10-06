#ifndef EQUIPO_H
#define EQUIPO_H

#include "utils.h"
#include "gameMaster.h"

#include <thread>
#include <semaphore.h>
#include <vector>

struct Equipo {
  Equipo(color equipo, estrategia strat, int quantum, struct GameMaster& belcebu, struct Config& config);

  void comenzar(void);
  void terminar(void);

  void jugador(int nroJugador);
  struct Pos buscarBanderaContraria(void);

  void secuencial(int nroJugador);
  void roundRobin(int nroJugador);
  void shortestDistanceFirst(int nroJugador);
  void ustedes(int nroJugador);

  int moverseHacia(int nroJugador, struct Pos to);

  struct GameMaster *belcebu; 
  color equipo;
  estrategia strat;
  std::vector<std::thread> jugadores;
  std::vector<struct Pos> posiciones;

  sem_t seqSem; //U: Semáforo para la estrategia secuencial
  int quantum, quantumLeft;
};

#endif // EQUIPO_H
