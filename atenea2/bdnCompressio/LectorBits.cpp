#include "LectorBits.hpp"

LectorBits::LectorBits(std::string fitxer){
    comptadorBit = 0;
    entrada = fopen (fitxer.c_str(), "r");
    fread(llistaLectura, sizeof(__uint8_t), midaArray, entrada);
    limitBloc = (llistaLectura[0] << 8) | (llistaLectura[1]);
};

signed char LectorBits::llegeixBit(){
    if (comptadorBit == limitBloc){
        if (comptadorBit == numBits){
            comptadorBit = 0;
            fread(llistaLectura, sizeof(__uint8_t), midaArray, entrada);
            limitBloc = (llistaLectura[0] << 8) | (llistaLectura[1]);
            if (limitBloc == 0){
                fclose(entrada);
                return -1;
            }
        }
        else{
            fclose(entrada);
            return -1;
        }
    }

    signed char resultat = ((llistaLectura[comptadorBit/8 + bytesPadding]) >> (7 - (comptadorBit%8))) & 0x01;
    ++comptadorBit;
    return resultat;
};