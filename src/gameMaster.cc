#include "gameMaster.h"

GameMaster::GameMaster(struct Config& config) {
  jugadores[ROJO] = config.jugadores[ROJO];
  jugadores[AZUL] = config.jugadores[AZUL];

  tablero.resize(config.alto, std::vector<color> (config.ancho, VACIO)); //A: Inicio el tablero vacío
  tablero[config.banderas[ROJO].y][config.banderas[ROJO].x] = BANDERA_ROJA; //A: Pongo las banderas
  tablero[config.banderas[AZUL].y][config.banderas[AZUL].x] = BANDERA_AZUL;
  for (struct Pos& pos : jugadores[ROJO]) tablero[pos.y][pos.x] = ROJO; //A: Pongo a los jugadores
  for (struct Pos& pos : jugadores[AZUL]) tablero[pos.y][pos.x] = AZUL; //A: Pongo a los jugadores

  equipo = EMPIEZA;
  ganador = INDEFINIDO;

  barriers[ROJO] = new Barrier(config.cantJugadores);
  barriers[AZUL] = new Barrier(config.cantJugadores);

  barriers[equipo]->post(INDEFINIDO); //A: Empieza un equipo
}

void GameMaster::moverJugador(int nroJugador, direccion dir) {
  struct Pos pos = jugadores[equipo][nroJugador],
             newPos = pos.mover(dir);

  mtx.lock();
  if (ganador == INDEFINIDO) { //A: Sigue el juego
    bool valid = isEmpty(newPos) || hasFlag(newPos, contrincante(equipo));
    
    if (valid) {
      if (hasFlag(newPos, contrincante(equipo))) ganador = equipo; //A: Ganaron

      //A: Actualizo el tablero
      tablero[pos.y][pos.x] = VACIO;
      tablero[newPos.y][newPos.x] = equipo;
      jugadores[equipo][nroJugador] = newPos;
    }
  }
  mtx.unlock();
}

void GameMaster::terminoRonda(color _equipo) {
  mtx.lock();

  if (ganador == INDEFINIDO) { //A: Sigue el juego
    equipo = contrincante(_equipo); //A: Cambio de equipo
    barriers[equipo]->post(INDEFINIDO); //A: Le doy el turno
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

bool GameMaster::hasFlag(struct Pos pos, color _equipo) {
  //NOTA: Asume mtx.lock()
  return tablero[pos.y][pos.x] == ((_equipo == ROJO) ? BANDERA_ROJA : BANDERA_AZUL);
}
