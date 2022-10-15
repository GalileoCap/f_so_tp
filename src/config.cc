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

  posiciones[ROJO].resize(cantJugadores);
  posiciones[AZUL].resize(cantJugadores);

  for (int i = 0; i < cantJugadores; i++)
    config >> posiciones[ROJO][i].x >> posiciones[ROJO][i].y;
  for (int i = 0; i < cantJugadores; i++)
    config >> posiciones[AZUL][i].x >> posiciones[AZUL][i].y;

  //TODO: Check invalid values
  logMsg("CONFIG done\n"); //TODO: Better log
};
