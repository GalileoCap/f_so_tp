#include "gameMaster.h"
#include "utils.h"
#include <iostream>

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

  this->barriers[ROJO] = new Barrier(config.cantJugadores);
  this->barriers[AZUL] = new Barrier(config.cantJugadores);

  this->barriers[this->equipo]->post(); //A: Empieza un equipo
}

void GameMaster::moverJugador(const direccion dir, const int jugador) {
  struct Pos& pos = this->jugadores[equipo][jugador],
              newPos = moverseDireccion(pos, dir);

  if (ganador.load() == INDEFINIDO) { //A: Sigue el juego
    this->mtx.lock();
    bool valid = true;
    //TODO: isEmpty(newPos) || hasFlag(newPos, contrincante(this->equipo))
    //TODO: Other checks
    
    if (valid) {
      if (this->tablero[newPos.y][newPos.x] == BANDERA_ROJA) { //TODO: hasFlag(newPos, contrincante(this->equipo))
        this->ganador.store(this->equipo); //A: Ganaron
        std::cout << this->equipo << " " << jugador << " WIN" << std::endl;
      }

      this->tablero[pos.y][pos.x] = VACIO;
      pos = newPos; //NOTA: Lo cambia por referencia
      this->tablero[pos.y][pos.x] = equipo;
    }

    this->mtx.unlock();
  }
}

void GameMaster::terminoRonda(const color equipo) {
  this->mtx.lock();
  if (this->ganador.load() == INDEFINIDO) {
    this->turno++;
    this->equipo = contrincante(equipo);
    this->barriers[this->equipo]->post();
  } else {
    this->barriers[ROJO]->post();
    this->barriers[AZUL]->post();
  }
  this->mtx.unlock();
}
