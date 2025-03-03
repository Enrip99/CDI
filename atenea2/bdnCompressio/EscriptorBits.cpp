#include "EscriptorBits.hpp"

EscriptorBits::EscriptorBits(std::string fitxer){
    comptadorBit = 0;
    sortida = fopen (fitxer.c_str(), "w");
    llistaEscriure[0] = (__uint8_t) (numBits >> 8);
    llistaEscriure[1] = (__uint8_t) (numBits & 0x00FF);
    for (int i = bytesPadding; i < midaArray; ++i) llistaEscriure[i] = 0;
    //Els dos primers elements de l'array són la quantitat de bits/bloc.
    //Només hi haurà menys de numBits bits menor a l'ultim bloc, ho deixem ja així.
    //Seria més eficient ficar només un 0, indicant que venen numBits bits seguits
    //i només ficar un 1 seguit del nombre d'elements restants. Però me la bufa.
};

EscriptorBits::~EscriptorBits(){
    if (comptadorBit){
        llistaEscriure[0] = (__uint8_t) (comptadorBit >> 8);
        llistaEscriure[1] = (__uint8_t) (comptadorBit & 0x00FF);
        //Escrivim el nombre de bits al bloc final
    }
    else{
        llistaEscriure[0] = (__uint8_t) 0x00;
        llistaEscriure[1] = (__uint8_t) 0x00;
        //escriure 00
    }

    fwrite(llistaEscriure, sizeof(__uint8_t), bytesPadding + ((comptadorBit + 8 - 1) / 8), sortida);
    //Escrivim el que queda al búfer
    //(A + B - 1)/B -> ceiling(A/B)
    fclose(sortida);

}

void EscriptorBits::escriuBit(bool bit){
    if (bit) llistaEscriure[comptadorBit/8 + bytesPadding] |= (__uint8_t) bit << (7 - (comptadorBit%8));
    //Activa el bit indicat en cas de ser 1.
    //Els dos primers bytes es reserven per la mida del bloc.
    //bit << (7 - (comptadorBit%8)) permet accedir primer als bits de més pes.
    ++comptadorBit;

    if (comptadorBit == numBits){
        //Escriure a disc en cas d'exhaurir el límit de bits/bloc.
        fwrite(llistaEscriure, sizeof(__uint8_t), midaArray, sortida);
        comptadorBit = 0;
        for (int i = bytesPadding; i < midaArray; ++i) llistaEscriure[i] = 0;
        //Reiniciem el bloc sencer a 0.
        //Els dos primer bytes es deixen iguals, només seràn diferens l'ultim bloc.
    }    
};