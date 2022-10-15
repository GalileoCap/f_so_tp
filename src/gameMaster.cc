#include "gameMaster.h"

GameMaster::GameMaster(const class Config& config) {
  tablero.resize(config.alto, std::vector<color> (config.ancho, VACIO)); //A: Inicio el tablero vacío

  for (color equipo : { ROJO, AZUL}) {
    tablero[config.banderas[equipo].y][config.banderas[equipo].x] = bandera(equipo); //A: Pongo la bandera
    posiciones[equipo] = config.posiciones[equipo];
    for (const struct Pos& pos : posiciones[equipo]) tablero[pos.y][pos.x] = equipo; //A: Pongo a los jugadores
    barriers[equipo] = new Barrier(config.cantJugadores);
  }

  currEquipo = EMPIEZA;
  ganador = INDEFINIDO;
  barriers[currEquipo]->post(); //A: Empieza un equipo
  sem_init(&semStart, 1, 1);

  logMsg("GAMEMASTER done currEquipo=%i\n", currEquipo);
  logTablero();
}

void GameMaster::moverJugador(direccion dir, int nroJugador) {
  mtx.lock();
  struct Pos pos = posiciones[currEquipo][nroJugador],
             newPos = pos.mover(dir);
  
  //TODO: assert newPos
  if (ganador == INDEFINIDO) { //A: Sigue el juego
    bool valid = mePuedoMover(pos, dir);
    //TODO: assert valid
    
    if (!valid) { logMsg("GAMEMASTER moverJugador INVALIDO\n"); while (true); }
    if (hasFlag(newPos, contrincante(currEquipo))) ganador = currEquipo; //A: Ganaron

    //A: Actualizo el tablero
    posiciones[currEquipo][nroJugador] = newPos;
    tablero[pos.y][pos.x] = VACIO;
    tablero[newPos.y][newPos.x] = currEquipo;
  }

  mtx.unlock();
}

void GameMaster::terminoRonda(color equipo) {
  mtx.lock();

  logMsg("GAMEMASTER terminoRonda equipo=%i, ganador=%i\n", equipo, ganador);
  logTablero();
  if (ganador == INDEFINIDO) { //A: Sigue el juego
    currEquipo = contrincante(equipo); //A: Cambio de equipo
    barriers[currEquipo]->post(); //A: Le doy el turno
  } else { //A: Se terminó el juego
    barriers[ROJO]->post(); //A: Le aviso a ambos equipos
    barriers[AZUL]->post();
  }

  mtx.unlock();
}

color GameMaster::enPosicion(const struct Pos &pos) {
  return tablero[pos.y][pos.x];
}

bool GameMaster::isEmpty(const struct Pos& pos) {
  return enPosicion(pos) == VACIO;
}

bool GameMaster::hasFlag(const struct Pos& pos, color equipo) {
  return enPosicion(pos) == bandera(equipo);
}

bool GameMaster::esPosicionValida(const struct Pos& pos) {
  return 0 <= pos.y && pos.y < tablero.size() && 0 <= pos.x && pos.x < tablero[0].size();
}

bool GameMaster::mePuedoMover(struct Pos pos, direccion dir) {
  pos = pos.mover(dir);
  return esPosicionValida(pos) && (isEmpty(pos) || hasFlag(pos, contrincante(currEquipo)));
}

void GameMaster::waitTurn(color equipo) {
  //TODO: assert color
  barriers[equipo]->wait();
}

color GameMaster::getGanador(void) {
  mtx.lock();
  color res = ganador;
  mtx.unlock();
  return res;
}

void GameMaster::logTablero(void) {
  for (int i = 0; i < tablero.size(); i++) {
    for (int j = 0; j < tablero[0].size(); j++) {
      color x = enPosicion({j, i});
      if (x != VACIO) logMsg("%i", x);
      else logMsg("_");
    }
    logMsg("\n");
  }
  logMsg("\n");
}
