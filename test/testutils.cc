#include "testutils.h"

void createConfig(int ancho, int alto, int cantJugadores, struct Pos banderas[2], std::vector<struct Pos> posiciones[2]) {
  std::ofstream config(CONFIG_FPATH);
  config << ancho << " " << alto << " " 
         << cantJugadores << std::endl
         << banderas[ROJO].x << " " << banderas[ROJO].y << std::endl
         << banderas[AZUL].x << " " << banderas[AZUL].y;
  for (color equipo : {ROJO, AZUL})
    for (const struct Pos& pos : posiciones[equipo])
      config << std::endl << pos.x << " " << pos.y;
}

tablero_t simpleSetup(void) {
  int ancho = 2, alto = 2, cantJugadores = 1;
  struct Pos banderas[2] = {{0, 0}, {1, 1}};
  std::vector<struct Pos> posiciones[2] = {
    {{1, 0}},
    {{0, 1}}
  };
  createConfig(ancho, alto, cantJugadores, banderas, posiciones);
  return tablero_t {
    {BANDERA_ROJA, ROJO},
    {AZUL, BANDERA_AZUL}
  };
}

tablero_t fullSetup(void) {
  int ancho = 3, alto = 3, cantJugadores = 2;
  struct Pos banderas[2] = {{0, 0}, {2, 2}};
  std::vector<struct Pos> posiciones[2] = {
    {{1, 0}, {2, 0}},
    {{0, 2}, {1, 2}}
  };
  createConfig(ancho, alto, cantJugadores, banderas, posiciones);
  return tablero_t {
    {BANDERA_ROJA, ROJO, ROJO},
    {VACIO, VACIO, VACIO},
    {AZUL, AZUL, BANDERA_AZUL}
  };
}
