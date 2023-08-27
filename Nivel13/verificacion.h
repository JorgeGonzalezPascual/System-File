//Autores: Jorge González Pascual, Luis Clar Fiol
#include "simulacion.h"

struct INFORMACION {
  int pid;
  unsigned int nEscrituras; //validadas ±50
  struct REGISTRO PrimeraEscritura;
  struct REGISTRO UltimaEscritura;
  struct REGISTRO MenorPosicion;
  struct REGISTRO MayorPosicion;
};
