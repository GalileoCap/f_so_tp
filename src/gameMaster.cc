#include "gameMaster.h"
#include <assert.h>

GameMaster::GameMaster(const class Config& config) {
  tablero.resize(config.alto, std::vector<color> (config.ancho, VACIO)); //A: Inicio el tablero vacío

  for (color equipo : { ROJO, AZUL}) { //A: Guardo los datos de ambos equipos
    tablero[config.banderas[equipo].y][config.banderas[equipo].x] = bandera(equipo); //A: Pongo la bandera
    posiciones[equipo] = config.posiciones[equipo];
    for (const struct Pos& pos : posiciones[equipo]) tablero[pos.y][pos.x] = equipo; //A: Pongo a los jugadores
    barriers[equipo] = new Barrier(equipo, config.cantJugadores);
    movidas[equipo] = 0;
  }

  atentoMovidas = false;
  currEquipo = EMPIEZA;
  ganador = INDEFINIDO;
  sem_init(&semBandera, 1, 1); //A: Dejo que un sólo equipo busque la bandera
  barriers[currEquipo]->post(ganador); //A: Dejo que arranque un equipo

  logMsg("[GameMaster] DONE currEquipo=%i\n", currEquipo);
  //logTablero();
}

void GameMaster::moverJugador(direccion dir, int nroJugador) {
  struct Pos pos = posiciones[currEquipo][nroJugador],
             newPos = pos.mover(dir);
  
  if (ganador == INDEFINIDO) { //A: Sigue el juego
    assert(mePuedoMover(pos, dir)); //A: El jugador se tendría que asegurar que se valga
    
    if (hasFlag(newPos, contrincante(currEquipo))) //A: Ganaron
      ganador = currEquipo;

    //A: Actualizo el tablero
    posiciones[currEquipo][nroJugador] = newPos;
    tablero[pos.y][pos.x] = VACIO;
    tablero[newPos.y][newPos.x] = currEquipo;
    movidas[currEquipo]++;
  }
}

void GameMaster::terminoRonda(color equipo) {
  logMsg("[terminoRonda] equipo=%i, ganador=%i\n", equipo, ganador);
  logTablero();

  assert(equipo == currEquipo); //A: Solo nos puede pedir terminar el equipo al que le toca
  if (movidas[equipo] == 0) { //A: Nadie se movió este turno
    if (atentoMovidas) ganador = EMPATE; //A: Nadie se movió en dos turnos
    else atentoMovidas = true;
  } else atentoMovidas = false;

  currEquipo = contrincante(equipo); //A: Cambio de equipo
  if (ganador == INDEFINIDO) { //A: Sigue el juego
    movidas[currEquipo] = 0;
    barriers[currEquipo]->post(ganador); //A: Le doy el turno
  } else { //A: Se terminó el juego
    barriers[ROJO]->post(ganador); //A: Dejo pasara a ambos equipos
    barriers[AZUL]->post(ganador);
  }
}

bool GameMaster::mePuedoMover(struct Pos pos, direccion dir) {
  pos = pos.mover(dir);
  return esPosicionValida(pos) && (isEmpty(pos) || hasFlag(pos, contrincante(currEquipo)));
}

color GameMaster::waitTurn(color equipo) {
  return barriers[equipo]->wait();
}

void GameMaster::tableroSize(int& height, int& width) {
  height = tablero.size();
  width = (height > 0) ? tablero[0].size() : 0;
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
