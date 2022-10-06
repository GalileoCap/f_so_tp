#include "config.h"
#include <iostream>
#include <fstream>

Config::Config(void) {
  std::ifstream config(CONFIG_FPATH);
  if (!config) {
    std::cerr << "Error: el archivo no pudo ser abierto" << std::endl;
    exit(1);
  }
  config >> this->ancho >> this->alto
    >> this->cantJugadores
    >> this->banderas[ROJO].x >> this->banderas[ROJO].y 
    >> this->banderas[AZUL].x >> this->banderas[AZUL].y;

  this->jugadores[ROJO].resize(this->cantJugadores);
  this->jugadores[AZUL].resize(this->cantJugadores);

  for (int i = 0; i < this->cantJugadores; i++)
    config >> this->jugadores[ROJO][i].x >> this->jugadores[ROJO][i].y;
  for (int i = 0; i < this->cantJugadores; i++)
    config >> this->jugadores[AZUL][i].x >> this->jugadores[AZUL][i].y;
};
