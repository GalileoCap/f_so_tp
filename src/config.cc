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

  for (color equipo : {ROJO, AZUL}) { //A: Leo las posiciones de ambos equipos
    posiciones[equipo].resize(cantJugadores);
    for (int i = 0; i < cantJugadores; i++) {
      config >> posiciones[equipo][i].x >> posiciones[equipo][i].y;
    }
  }

  //TODO: Check invalid values
  logMsg("CONFIG done\n"); //TODO: Better log
};
