#ifndef EQUIPO_H
#define EQUIPO_H

#include "utils.h"
#include "gameMaster.h"

#include <thread>
#include <vector>

struct Equipo {
  Equipo(color equipo, estrategia strat, int quantum, struct GameMaster& belcebu, struct Config& config);

  void comenzar(void);
  void terminar(void);

  void jugador(const int nroJugador);
  struct Pos buscarBanderaContraria(void);

  struct GameMaster *belcebu; 
  color equipo;
  estrategia strat;
  std::vector<std::thread> jugadores;
  std::vector<struct Pos> posiciones;

  int quantum, quantumLeft;
};

#endif // EQUIPO_H
