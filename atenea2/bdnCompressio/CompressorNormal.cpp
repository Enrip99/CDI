#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include "EscriptorBits.hpp"
#include "DimensionsIO.hpp"
#include "BinTree.hpp"


void escriuArbre(const BinTree<__uint8_t> & arbre, EscriptorBits & sortida)
{
    if (arbre.left().empty()){
        sortida.escriuBit(1);
        sortida.escriuBit(arbre.value() & 0b10000000);
        sortida.escriuBit(arbre.value() & 0b01000000);
        sortida.escriuBit(arbre.value() & 0b00100000);
        sortida.escriuBit(arbre.value() & 0b00010000);
        sortida.escriuBit(arbre.value() & 0b00001000);
        sortida.escriuBit(arbre.value() & 0b00000100);
        sortida.escriuBit(arbre.value() & 0b00000010);
        sortida.escriuBit(arbre.value() & 0b00000001);
    }
    else{
        sortida.escriuBit(0);
        escriuArbre(arbre.left(),  sortida);
        escriuArbre(arbre.right(), sortida);
    }
}

void creaBufferIntern(const BinTree<__uint8_t> & arbre, std::map<__uint8_t, std::vector<bool> > & cacheArbre, std::vector<bool> & codificacio){
    if (arbre.left().empty()){
        cacheArbre[arbre.value()] = codificacio;
    }
    else{
        codificacio.push_back(false);
        creaBufferIntern(arbre.left(), cacheArbre, codificacio);
        codificacio.back() = true;
        creaBufferIntern(arbre.right(), cacheArbre, codificacio);
        codificacio.pop_back();
    }
}

void creaBuffer(const BinTree<__uint8_t> & arbre, std::map<__uint8_t, std::vector<bool> > & cacheArbre){
    std::vector<bool> tempCodificacio;
    creaBufferIntern(arbre, cacheArbre, tempCodificacio);
}

int main(int argc, char *argv[]){
    
    if (argc != 3){
        std::cout << "Us: " << argv[0] << " <input> <output>" << std::endl;
        exit(1);
    }
    

    unsigned long long int llistatOcurrencies[bytesDiferents] = {0};
    //Per a cada byte 0x00 - 0xFF, comptador de quants cops surt.

    FILE * entrada = fopen(argv[1], "r");
    __uint8_t bufferLectura[midaBuffer];
    int bytesLlegits;
    //Obrim fitxer a llegir, primer argument de la linia de comanda.
    //Variables per assistir a la lectura a baix nivell.

    std::multimap<unsigned long long int, BinTree<__uint8_t> > insercions;
    std::multimap<unsigned long long int, BinTree<__uint8_t> >::iterator iterador;
    unsigned long long int tempOcurrencia;
    BinTree<__uint8_t> tempArbre;
    //Mapa auto-ordenat amb les fulles o sub-arbres que formaran el huffman.

    BinTree<__uint8_t> ArbreHuffman;
    //Arbre final.

    EscriptorBits escriptor(argv[2]);
    //Objecte per escriure l'arbre i fixter comprimit a disc, segon argument de la linia de comanda.

    std::map<__uint8_t, std::vector<bool> > cacheArbre;
    //Mapa amb els bits que corresponen a cada byte per poder comprimir sense recòrrer l'arbre.


    //Llistat amb les freqüències de cada byte.
    //
    //Recorrem el fitxer original sencer.
    //Per cada ocurrència d'un byte, s'incrementa en 1 el seu comptador a l'array.
int totalcount = 0;
    while ((!std::feof(entrada)) && (bytesLlegits = fread(bufferLectura, sizeof(__uint8_t), midaBuffer, entrada))){
        for (int i = 0; i < bytesLlegits; ++i){
            ++llistatOcurrencies[bufferLectura[i]];
            ++totalcount;
        }
    }
    //Construir arbre.
    //
    //Per cada element no zero al llistat de freqüències es crea un element d'un multimap.
    //Cada element s'identifica per la freqüència, i compté un arbre (només fulla) amb el byte
    //al que correspon.

    for (unsigned int i = 0; i < bytesDiferents; ++i){
        if (llistatOcurrencies[i]){
            insercions.insert({
                llistatOcurrencies[i],
                BinTree<__uint8_t>((__uint8_t)(i & 0xFF))
            });
        }
    }

    //Reiteradament, agafem els dos primers elements del mapa i fusionem els seus dos arbres.
    //Eliminem aquests dos elements i fiquem el nou arbre, amb la freqüència essent la
    //suma dels dos sub-arbres originals. Repetim fins que només en quedi un.
    //Desem l'arbre final a ArbreHuffman.

    iterador = insercions.begin();
    while (insercions.size() > 1){
        tempOcurrencia = iterador -> first;
        tempArbre = iterador -> second;
        iterador = insercions.erase(iterador);
        insercions.insert({
            tempOcurrencia + iterador -> first,
            BinTree<__uint8_t>(0xFF, tempArbre, iterador -> second)
            });
        iterador = insercions.erase(iterador);
    }
    ArbreHuffman = iterador -> second;


    //Escriure arbre a disc
    //
    //Escrivim l'arbre en inordre. Un 0 significa baixar per la branca esquerra.
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

    escriuArbre(ArbreHuffman, escriptor);


    //Llegir un altre cop el fitxer i comprimir-ho.
    //
    //Primer creem un "buffer" a un mapa per poder accedir més ràpidament a la codificació de cada element sense
    //recorrer l'arbre sencer cada cop cercant per l'element.
    //Cada element del mapa conté un vector<bool> corresponent a la seva codificació.
    //Per l'exemple anterior:
    //  cacheArbre['A'] == vector<bool> {0,0}
    //  cacheArbre['B'] == vector<bool> {0,1}
    //  cacheArbre['C'] == vector<bool> {1,0}
    //  cacheArbre['D'] == vector<bool> {1,1,0}
    //  cacheArbre['E'] == vector<bool> {1,1,1}

    creaBuffer(ArbreHuffman, cacheArbre);

    std::map<__uint8_t, std::vector<bool> >::iterator it;
float xd = 0.0;
for (it = cacheArbre.begin(); it != cacheArbre.end(); it++)
{
    xd += it->second.size() * llistatOcurrencies[it->first];
    std::cout << it->first << ": ";
    for (int i = 0; i < it->second.size(); ++i){
        if (it->second[i]) std::cout << "0";
        else std::cout << "1";
    }
    std::cout << " - " << xd << std::endl ;
}

    xd /= (float) totalcount;

    std::cout << xd;


    //Tornem a recorrer el fitxer original. Per a cada byte, mirem la seva codificació al búffer, i l'escrivim al fitxer de sortida.
    
    std::fseek(entrada, 0, SEEK_SET);
    std::clearerr(entrada);
    while ((!std::feof(entrada)) && (bytesLlegits = fread(bufferLectura, sizeof(__uint8_t), midaBuffer, entrada))){
        for (int i = 0; i < bytesLlegits; ++i){
            for (int j = 0; j < cacheArbre[bufferLectura[i]].size(); ++j){
                escriptor.escriuBit(cacheArbre[bufferLectura[i]][j]);
            }
        }
    }
    
    std::fclose(entrada);
    
    
    return 0;
}