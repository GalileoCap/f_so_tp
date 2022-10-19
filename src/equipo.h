#ifndef __EQUIPO_H__
#define __EQUIPO_H__

#include "gameMaster.h"
#include "utils.h"

#include <thread>
#include <mutex>
#include <vector>

class Equipo {
public:
  Equipo(
    class GameMaster *belcebu,
    color equipo, estrategia strat, int cantJugadores, int quantum,
    const std::vector<struct Pos>& posiciones
  );

  void comenzar(void); //U: Busca las banderas y luego inicia a sus jugadores
  void terminar(void); //U: Espera a que terminen todos los jugadores de este equipo

#ifndef TEST
private:
#endif // TEST
  void jugador(int nroJugador);
  void moverse(int nroJugador, const struct Pos& to); //U: Mueve al jugador hacia una posición
  bool esperarBelcebu(void);

  void buscarBanderas(void); //U: Busca ambas banderas en el tablero
  void buscarThread(int height, int from, int to, bool& foundOurs, bool& foundEnemy); //U: Cada thread recorre una porción del tablero hasta terminar o que entre todos hayan encontrado ambas banderas

  class GameMaster *belcebu; 
  color equipo;
  estrategia strat;
  std::vector<struct Pos> posiciones;
  std::vector<std::thread> threads;
  int quantum, quantumLeft;
  struct Pos banderas[2];

  //******************************
  //S: Estrategias
 
  void secuencial(int nroJugador);
  std::mutex seq_mtx;
  int seq_turno; //U: Cantidad de jugadores que ya se movieron

  void roundRobin(int nroJugador);
  std::vector<std::mutex*> rr_mtx; //TODO: No punteros, tira un error de compilación si no lo son
  int rr_last;

  void shortestDistanceFirst(int nroJugador);
  std::vector<int> sdf_distances;
  sem_t sdf_step[2];
  std::mutex sdf_mtx;
  int sdf_turno, sdf_closest;

  void ustedes(int nroJugador);
  std::mutex us_mtx, us_moveMtx;
  int us_range[2], us_count;
};

#endif // EQUIPO_H
