#include "gameMaster.h"
#include "equipo.h"
#include "config.h"
#include "utils.h"

#include <iostream>

const estrategia strat = SECUENCIAL;
const int quantum = 10;

int main(void) {
  struct Config config;
  struct GameMaster belcebu(config);

  //A: Creo equipos
  struct Equipo rojo(ROJO, strat, quantum, belcebu, config),
                azul(AZUL, strat, quantum, belcebu, config);

  rojo.comenzar();
  azul.comenzar();
  rojo.terminar();
  azul.terminar();	

  std::cout << "Bandera capturada por el equipo " << belcebu.ganador << ". Felicidades!" << std::endl;
  return 0;
}

