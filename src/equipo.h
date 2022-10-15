#ifndef __EQUIPO_H__
#define __EQUIPO_H__

#include "gameMaster.h"
#include "utils.h"

#include <thread>
#include <mutex>
#include <vector>

class Equipo {
public:
  Equipo(class GameMaster *belcebu, color equipo, estrategia strat, int cantJugadores, int quantum, const std::vector<struct Pos>& posiciones);

  void comenzar(void);
  void terminar(void);

private:
  void jugador(int nroJugador);
  void moverse(int nroJugador);

  struct Pos buscarBanderaContraria(void);
  void buscarThread(int height, int from, int to, bool& foundOurs, bool& foundEnemy);

  class GameMaster *belcebu; 
  color equipo;
  estrategia strat;
  std::vector<struct Pos> posiciones;
  std::vector<std::thread> threads;
  int quantum, quantumLeft;
  struct Pos banderas[2];

  //******************************
  //S: Estrategias
  //U: Secuencial
  void secuencial(int nroJugador);
  std::mutex seq_mtx;
  int seq_turno; //U: Cantidad de jugadores que ya se movieron

  //U: RR
  void roundRobin(int nroJugador);
  std::vector<std::mutex*> rr_mtx;
  int rr_last;

  //U: SDF
  void shortestDistanceFirst(int nroJugador);

  //U: Nuestra
  void ustedes(int nroJugador);
};

#endif // EQUIPO_H
