#ifndef __EQUIPO_H__
#define __EQUIPO_H__

#include "utils.h"
#include "gameMaster.h"

#include <thread>
#include <mutex>
#include <vector>

class Equipo {
  Equipo(color _equipo, estrategia _strat, int _quantum, class GameMaster *_belcebu, class Config& config);

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

  class GameMaster *belcebu; 
  color equipo;
  estrategia strat;
  std::vector<struct Pos> posiciones;
  std::vector<std::thread> threads;

  std::mutex seq_mtx; //U: Mutex para la estrategia secuencial
  int seq_turno; //U: Cantidad de jugadores que ya se movieron en la estrategia secuencial

  std::vector<std::mutex*> rr_mtx;
  int rr_last;

  int quantum, quantumLeft;

  friend class GameMaster;
};

#endif // EQUIPO_H
