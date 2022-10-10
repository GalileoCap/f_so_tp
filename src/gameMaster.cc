#include "gameMaster.h"

GameMaster::GameMaster(estrategia strat, int quantum, class Config& config) {
  equipos[ROJO] = new Equipo(ROJO, strat, quantum, this, config);
  equipos[AZUL] = new Equipo(AZUL, strat, quantum, this, config);

  tablero.resize(config.alto, std::vector<color> (config.ancho, VACIO)); //A: Inicio el tablero vacío
  tablero[config.banderas[ROJO].y][config.banderas[ROJO].x] = BANDERA_ROJA; //A: Pongo las banderas
  tablero[config.banderas[AZUL].y][config.banderas[AZUL].x] = BANDERA_AZUL;
  for (struct Pos& pos : config.jugadores[ROJO]) tablero[pos.y][pos.x] = ROJO; //A: Pongo a los jugadores
  for (struct Pos& pos : config.jugadores[AZUL]) tablero[pos.y][pos.x] = AZUL; //A: Pongo a los jugadores

  currEquipo = EMPIEZA;
  ganador = INDEFINIDO;

  barriers[ROJO] = new Barrier(config.cantJugadores);
  barriers[AZUL] = new Barrier(config.cantJugadores);

  barriers[currEquipo]->post(ganador); //A: Empieza un equipo
}

void GameMaster::comenzar(void) {
  equipos[ROJO]->comenzar();
  equipos[AZUL]->comenzar();
}

int GameMaster::terminar(void) {
  equipos[ROJO]->terminar();
  equipos[AZUL]->terminar();
  return ganador;
}

void GameMaster::moverJugador(int nroJugador, direccion dir) {
  struct Pos pos = equipos[currEquipo]->posiciones[nroJugador],
             newPos = pos.mover(dir);

  mtx.lock();
  if (ganador == INDEFINIDO) { //A: Sigue el juego
    bool valid = isEmpty(newPos) || hasFlag(newPos, contrincante(currEquipo));
    
    if (valid) {
      if (hasFlag(newPos, contrincante(currEquipo))) ganador = currEquipo; //A: Ganaron

      //A: Actualizo el tablero
      tablero[pos.y][pos.x] = VACIO;
      tablero[newPos.y][newPos.x] = currEquipo;
      equipos[currEquipo]->posiciones[nroJugador] = newPos;
    }
  }
  mtx.unlock();
}

void GameMaster::terminoRonda(color equipo) {
  mtx.lock();

  if (ganador == INDEFINIDO) { //A: Sigue el juego
    currEquipo = contrincante(equipo); //A: Cambio de equipo
    barriers[currEquipo]->post(INDEFINIDO); //A: Le doy el turno
  } else { //A: Se terminó el juego
    barriers[ROJO]->post(ganador); //A: Le aviso a ambos equipos
    barriers[AZUL]->post(ganador);
  }
  mtx.unlock();
}

bool GameMaster::isEmpty(struct Pos pos) {
  //NOTA: Asume mtx.lock()
  return tablero[pos.y][pos.x] == VACIO;
}

bool GameMaster::hasFlag(struct Pos pos, color equipo) {
  //NOTA: Asume mtx.lock()
  return tablero[pos.y][pos.x] == ((equipo == ROJO) ? BANDERA_ROJA : BANDERA_AZUL);
}

int GameMaster::waitTurn(color equipo) {
  return barriers[equipo]->wait();
}
