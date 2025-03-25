#include "BinTree.hpp"
#include <vector>
#include <optional>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>

#define MAX_BLOC 65536
#define ELEMENTS_HUFFMAN 257

std::ofstream fitxerSortida; // Fitxer de sortida a disc.
uint8_t byteActualBloc; // Byte codificat amn l'algorisme propietarique s'envia al Huffman.
uint8_t byteActualDisc; // Byte codificat en huffman que enviem a disc.
int bitsActualsBloc; // Nombre de bits escrits al byteActualBloc.
int bitsActualsDisc; // Nombre de bits escrits al byteActualDisc.
FILE * fitxerEntrada; // Fitxer d'entrada de disc.
uint32_t midaBloc; // Numero de bytes llegits del bloc actual.
uint8_t * bloc = new uint8_t[MAX_BLOC]; // Bloc on desem temporalment els bits descodificats per Huffman de forma propietària abans de ser descodificats per segon cop.

// Actualitza byteActualDisc amb el següent byte de disc.
void llegeixByteDeDisc(){
    fread(& byteActualDisc, sizeof(uint8_t), 1, fitxerEntrada);
}

// Retorna el proper bit desat a disc.
bool llegeixBitDeDisc(){
    if (bitsActualsDisc == 0){
        llegeixByteDeDisc();
        bitsActualsDisc = 8;
    }
    return (byteActualDisc >> (--bitsActualsDisc)) & 1;
}

// Retorna els propers quantitat bits desats a disc.
// Funcionament no definit per a quantitat > (sizeof(unsigned long) * 8).
unsigned long llegeixBitsDeDisc(int quantitat){
    unsigned long temporal = 0;
    while (quantitat--){
        temporal |= llegeixBitDeDisc() << quantitat;
    }
    return temporal;
} 

// Llegeix i decodifica el proper bloc de disc.
// Deixa els bytes llegits a bloc.
// Actualitza midaBloc amb el nombre de bytes llegits.
// Retorna false si troba cap EOF.
// Altrament, retorna true.
bool llegeixBlocDeDisc(){

    std::vector <int> arbreLong (ELEMENTS_HUFFMAN, 0), bl_count (ELEMENTS_HUFFMAN, 0);
    std::vector <unsigned long> /*arbreCodi (ELEMENTS_HUFFMAN, 0),*/ properCodi (ELEMENTS_HUFFMAN, 0);
    std::map <std::pair<unsigned long, int>, int> arbreCodi;

    // arbreLong := longitud de codi de cada byte <--
    // bl_count := nombre de bytes amb codi mida N
    // arbreCodi := codi real de cada byte <--
    // properCodi := variable auxiliar

    // Obtenim les longituds de codi per poder refer l'arbre
    for (int i = 0; i < ELEMENTS_HUFFMAN; ++i){
        int longitudCodi = llegeixBitsDeDisc(5);
        if (longitudCodi == 0){
            longitudCodi = llegeixBitsDeDisc(8);
            if (longitudCodi == 0) return false;
            longitudCodi += 31;
        }
        arbreLong[i] = longitudCodi;
    }

    // Calculem quants cops apareix cada longitud.
    int maxLong = 0, codi = 0;
    for (int i = 0; i < ELEMENTS_HUFFMAN; ++i){
        if (arbreLong[i] > maxLong) maxLong = arbreLong[i];
        bl_count[arbreLong[i]]++;
    }

    // Calculem el valor numèric del codi més petit per a cada longitud de codi.
    for (int i = 1; i <= maxLong; ++i){
        codi = (codi + bl_count[i - 1]) << 1;
        properCodi[i] = codi;
    }

    // Per a cada símbol, li asignem el seu codi de forma numèrica.
    // codi = codi mínim per a la longitud del codi del símbol
    //        + nombre de cops que s'ha asignat ja un codi d'aqusta mida
    for (int i = 0; i < ELEMENTS_HUFFMAN; ++i){
        int longit = arbreLong[i];
        if (longit){
            //arbreCodi[i] = properCodi[longit];
            arbreCodi[std::pair<unsigned long, int>(properCodi[longit], longit)] = i;
            properCodi[longit]++;
        }
    }

    midaBloc = 0;
    unsigned long codiLlegint = 0;
    int midaCodiLlegint = 0;

    while (1){
        bool bit = llegeixBitDeDisc();
        codiLlegint = (codiLlegint << 1) + bit;
        ++midaCodiLlegint;
        if (auto cerca = arbreCodi.find(std::pair(codiLlegint,midaCodiLlegint)); cerca != arbreCodi.end()){
            if (cerca->second == ELEMENTS_HUFFMAN - 1){
                return true;
            }
            else{
                bloc[midaBloc++] = cerca->second;
                codiLlegint = 0;
                midaCodiLlegint = 0;
            }
        }
    }
}

int main (int argc, char ** argv){

    // Comprovació dels paràmetres d'entrada.

    if (argc != 3){
        std::string misError (argv[0]);
        std::cerr << "Arguments incorrectes.\nUs: " + misError + " entrada.txt sortida.knk" << std::endl;
        exit(1);
    }

    if (strcmp(argv[1], argv[2]) == 0){
        std::cerr << "Mateix argument d'entrada que de sortida." << std::endl;
        exit(1);
    }

    // Preparem fitxer d'entrada.
    fitxerEntrada = fopen (argv[1], "r");

    if (!fitxerEntrada){
        perror("Error obrint fitxer d'entrada");
        exit(1);
    }

    // Preparem fitxer d'escriptura.
    fitxerSortida = std::ofstream(argv[2]);

    if (!fitxerSortida){
        perror("Error obrint fitxer de sortida");
        exit(1);
    }

    // Iniciem algunes variables globals a zero.
    byteActualBloc = 0;
    byteActualDisc = 0;
    bitsActualsBloc = 0;
    bitsActualsDisc = 0;
    
    while (llegeixBlocDeDisc()){

    }

}