#include "gameMaster.h"
#include "utils.h"

GameMaster::GameMaster(struct Config& config) {
  this->jugadores[ROJO] = config.jugadores[ROJO];
  this->jugadores[AZUL] = config.jugadores[AZUL];

  this->tablero.resize(config.alto, std::vector<color> (config.ancho, VACIO)); //A: Inicio el tablero vacío
  this->tablero[config.banderas[ROJO].y][config.banderas[ROJO].x] = BANDERA_ROJA; //A: Pongo las banderas
  this->tablero[config.banderas[AZUL].y][config.banderas[AZUL].x] = BANDERA_AZUL;
  for (struct Pos& pos : this->jugadores[ROJO]) this->tablero[pos.y][pos.x] = ROJO; //A: Pongo a los jugadores
  for (struct Pos& pos : this->jugadores[AZUL]) this->tablero[pos.y][pos.x] = AZUL; //A: Pongo a los jugadores

  this->equipo = EMPIEZA;
  this->ganador = INDEFINIDO;

  //TODO: Check for invalid values

  sem_init(&this->semaphores[ROJO], 1, 0);
  sem_init(&this->semaphores[AZUL], 1, 0);

  sem_post(&this->semaphores[EMPIEZA]); //A: Empieza un equipo
}

int GameMaster::moverJugador(const direccion dir, const int jugador) {
}

void GameMaster::terminoRonda(const color equipo) {
}
