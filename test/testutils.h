#ifndef __TESTUTILS_H__
#define __TESTUTILS_H__

#include <iostream>
#include <fstream>
#include <vector>

#include "barrier.h"
#include "utils.h"

using tablero_t = std::vector<std::vector<color>>;

void createConfig(int ancho, int alto, int cantJugadores, struct Pos banderas[2], std::vector<struct Pos> posiciones[2]);

tablero_t simpleSetup(void);
tablero_t fullSetup(void);

void consumeBarrier(class Barrier *barrier);

#endif // __TESTUTILS_H__
