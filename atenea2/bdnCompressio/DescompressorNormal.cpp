#include <iostream>
#include <string>
#include <stdio.h>
#include "LectorBits.hpp"
#include "DimensionsIO.hpp"
#include "BinTree.hpp"

void errorLectura(){
    perror("Error en llegir el fitxer.");
    exit(1);
}

void llegeixArbre(BinTree<__uint8_t>& arbre, LectorBits & lector){
    signed char llegit = lector.llegeixBit();
    if (llegit == 0){
        BinTree<__uint8_t> l;
        llegeixArbre(l, lector);
        BinTree<__uint8_t> r;
        llegeixArbre(r, lector);
        arbre = BinTree<__uint8_t>(0, l, r);
    }
    else if (llegit == 1){
        __uint8_t properByte = 0x00;
        for (int i = 7; i >= 0; --i){
            llegit = lector.llegeixBit();
            if (llegit == -1) errorLectura();
            properByte |= (llegit << i);
        }
        arbre = BinTree<__uint8_t>(properByte);
    }
    else errorLectura();
}

int main(int argc, char *argv[]){

    if (argc != 3){
        std::cout << "Us: " << argv[0] << " <input> <output>" << std::endl;
        exit(1);
    }
    

    LectorBits lector(argv[1]);
    char llegit;
    //Objecte per llegir el fixter comprimit a disc, primer argument de la linia de comanda.

    BinTree<__uint8_t> arbreHuffman, arbreRecorrer;
    //Objectes d'arbre que farem servir per descomprimir el fitxer i un d'auxiliar.

    FILE * sortida = fopen(argv[2], "w");
    __uint8_t bufferEscriptura[midaBuffer] = {0};
    int bytesAEscriure;
    //Obrim fitxer per escriure, segon argument de la linia de comanda.
    //Variables per assistir a l'escriptura a baix nivell.


    //Llegir arbre de disc
    //
    //Llegim l'arbre en inordre. Un 0 significa baixar per la branca esquerra.
    //Un 1 significa la presència d'una fulla. Després de cada 1 venen 8 bits representant
    //el byte que codifiquen. Després de cada fulla, es retrocedeix al node pare i es
    //continua per la branca dreta, sense cap bit que ho indiqui (recodem que en un arbre de
    //Huffman, tots el nodes tenen 0 o 2 branques, mai 1).
    //
    //      arrel
    //       / \
    //      -   -
    //     / \ / \
    //     A B C -
    //          / \
    //          D E
    //
    //Es representaria com 0 0 1(A) 1(B) 0 1(C) 0 1(D) 1(E)
    //Si el lector retorna un -1 en cap moment (EOF), interpretem que el fitxer s'ha
    //corrompit i avortem.

    llegeixArbre(arbreHuffman, lector);


    //Descomprimim la resta del fitxer.
    //
    //Creem una còpia de l'arbre de Huffman, i comencem a baixar per esquerra o dreta
    //si llegim un 0 o un 1, respectivament. Quan trobem una fulla, escrivim el seu valor
    //i tornem a començar des de dalt. Terminem quan el lector ens avisa d'EOF.
    //Per escriure a disc, cada cop que trobem per fi midaBuffer bytes; o quan acabem,
    //els escrivim alhora.

    arbreRecorrer = BinTree<__uint8_t> (arbreHuffman);
    bytesAEscriure = 0;
    while(llegit = lector.llegeixBit(), llegit != -1){
        if (llegit == 0){
            arbreRecorrer = arbreRecorrer.left();
        }
        else{
            arbreRecorrer = arbreRecorrer.right();
        }
        
        if (arbreRecorrer.left().empty()){
            bufferEscriptura[bytesAEscriure++] = arbreRecorrer.value();
            if (bytesAEscriure == midaBuffer){
                fwrite(bufferEscriptura, sizeof(__uint8_t), bytesAEscriure, sortida);
                bytesAEscriure = 0;
                for (int i = 0; i < midaBuffer; ++i) bufferEscriptura[i] = 0;
            }
            arbreRecorrer = BinTree<__uint8_t> (arbreHuffman);
        }
    }

    if (bytesAEscriure) std::fwrite(bufferEscriptura, sizeof(__uint8_t), bytesAEscriure, sortida);
    std::fclose(sortida);

    if (! arbreHuffman.equals(arbreRecorrer)){
        perror("S'ha arribat al final del fitxer sense descomprimir totes les dades.\n \
        El fitxer comprimit es podria haver corromput.");
        exit(1);
    }

    return 0;
}