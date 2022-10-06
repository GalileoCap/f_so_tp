#include "config.h"
#include <iostream>
#include <fstream>

Config::Config(void) {
  std::ifstream config(CONFIG_FPATH);
  if (!config) {
    std::cerr << "Error: el archivo no pudo ser abierto" << std::endl;
    exit(1);
  }
  config >> ancho >> alto
    >> cantJugadores
    >> banderas[ROJO].x >> banderas[ROJO].y 
    >> banderas[AZUL].x >> banderas[AZUL].y;

  jugadores[ROJO].resize(cantJugadores);
  jugadores[AZUL].resize(cantJugadores);

  for (int i = 0; i < cantJugadores; i++)
    config >> jugadores[ROJO][i].x >> jugadores[ROJO][i].y;
  for (int i = 0; i < cantJugadores; i++)
    config >> jugadores[AZUL][i].x >> jugadores[AZUL][i].y;

  //TODO: Check invalid values
};
