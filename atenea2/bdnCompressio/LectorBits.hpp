#ifndef LECTORBITS_H
#define LECTORBITS_H

#include <stdio.h>
#include <string>
#include "DimensionsIO.hpp"

class LectorBits{
    private:
        __uint8_t llistaLectura[midaArray];
        __uint16_t comptadorBit;
        __uint16_t limitBloc;
        FILE * entrada;

    public:
        //Constructora, entrada és un nom de fitxer
        LectorBits(std::string fitxer);

        //Llegeix bit del fitxer
        //Retorna 1 si el proper bit és true
        //Retorna 0 si el proper bit és false
        //Retorna -1 si EOF
        signed char llegeixBit();
};

#endif