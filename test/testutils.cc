#include "testutils.h"

void createConfig(int ancho, int alto, int cantJugadores, struct Pos banderas[2], std::vector<struct Pos> posiciones[2]) {
  std::ofstream config(CONFIG_FPATH);
  config << ancho << " " << alto << " " 
         << cantJugadores << std::endl
         << banderas[ROJO].x << " " << banderas[ROJO].y << std::endl
         << banderas[AZUL].x << " " << banderas[AZUL].y << std::endl;
  for (color equipo : {ROJO, AZUL})
    for (const struct Pos& pos : posiciones[equipo])
      config << std::endl << pos.x << " " << pos.y;
}
