#ifndef ESCRIPTORBITS_H
#define ESCRIPTORBITS_H

#include <stdio.h>
#include <string>
#include "DimensionsIO.hpp"

class EscriptorBits{
    private:
        __uint8_t llistaEscriure[midaArray];
        __uint16_t comptadorBit;
        FILE * sortida;

    public:
        //Constructora, sortida és un nom de fitxer
        EscriptorBits(std::string fitxer);

        //Desctructora, fa flush
        ~EscriptorBits();

        //Escriu el bit al fitxer
        void escriuBit(bool bit);
};

#endif