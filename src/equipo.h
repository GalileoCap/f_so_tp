#ifndef EQUIPO_H
#define EQUIPO_H

#include "utils.h"
#include "gameMaster.h"

#include <thread>
#include <semaphore.h>
#include <vector>

struct Equipo {
  struct Jugador {
    struct Pos pos;
    std::thread thread;
  };

  Equipo(color _equipo, estrategia _strat, int _quantum, struct GameMaster& _belcebu, struct Config& _config);

  void comenzar(void);
  void terminar(void);
  bool esperarBelcebu(void);

  void jugador(int nroJugador);
  void secuencial(int nroJugador);
  void roundRobin(int nroJugador);
  void shortestDistanceFirst(int nroJugador);
  void ustedes(int nroJugador);

  struct Pos buscarBanderaContraria(void);
  void moverseHacia(int nroJugador, struct Pos to);

  struct GameMaster *belcebu; 
  color equipo;
  estrategia strat;
  std::vector<std::thread> threads;

  sem_t seq_sem; //U: Semáforo para la estrategia secuencial
  int seq_turno; //U: Cantidad de jugadores que ya se movieron en la estrategia secuencial

  int quantum, quantumLeft;
};

#endif // EQUIPO_H
