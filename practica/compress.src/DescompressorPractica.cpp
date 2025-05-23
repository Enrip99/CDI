#ifndef DESCOMPRESSOR_PRAC_H
#define DESCOMPRESSOR_PRAC_H

#include "BinTree.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

#define MAX_BLOC 65536
#define ELEMENTS_HUFFMAN 257

class DescompressorPrac{
    public:
        void descomprimeix (char * paramEntrada, char * paramSortida);
    private:
        std::ofstream fitxerSortida; // Fitxer de sortida a disc.
        uint8_t byteActualBloc; // Byte codificat amn l'algorisme propietarique s'envia al Huffman.
        uint8_t byteActualDisc; // Byte codificat en huffman que enviem a disc.
        int bitsActualsBloc; // Nombre de bits escrits al byteActualBloc.
        int bitsActualsDisc; // Nombre de bits escrits al byteActualDisc.
        FILE * fitxerEntrada; // Fitxer d'entrada de disc.
        uint32_t midaBloc; // Mida del bloc actual.
        uint32_t posActualBloc; // Numero de bytes llegits del bloc actual.
        uint8_t bloc [MAX_BLOC]; // Bloc on desem temporalment els bits descodificats per Huffman de forma propietària abans de ser descodificats per segon cop.
        void llegeixByteDeDisc();
        bool llegeixBitDeDisc();
        unsigned long llegeixBitsDeDisc(int quantitat);
        void llegeixArbre(const BinTree<int> & arbre, std::string codi);
        bool llegeixBlocDeDisc();
        bool llegeixByteDelBloc();
        int llegeixBitDelBloc();
        unsigned long llegeixBitsDelBloc(int quantitat, bool & fiDeFitxer);

};




// Actualitza byteActualDisc amb el següent byte de disc.
void DescompressorPrac::llegeixByteDeDisc(){
    fread(& byteActualDisc, sizeof(uint8_t), 1, fitxerEntrada);
}

// Retorna el proper bit desat a disc.
bool DescompressorPrac::llegeixBitDeDisc(){
    if (bitsActualsDisc == 0){
        llegeixByteDeDisc();
        bitsActualsDisc = 8;
    }
    return (byteActualDisc >> (--bitsActualsDisc)) & 1;
}

// Retorna els propers quantitat bits desats a disc.
// Funcionament no definit per a quantitat > (sizeof(unsigned long) * 8).
unsigned long DescompressorPrac::llegeixBitsDeDisc(int quantitat){
    unsigned long temporal = 0;
    while (quantitat--){
        temporal |= llegeixBitDeDisc() << quantitat;
    }
    return temporal;
}

void DescompressorPrac::llegeixArbre(const BinTree<int> & arbre, std::string codi){
    if (arbre.left().empty()) {
        std::cout << codi << ": " << arbre.value() << std::endl;
        return;
    }
    llegeixArbre(arbre.left(), codi + '0');
    llegeixArbre(arbre.right(), codi + '1');
}

// Llegeix i decodifica el proper bloc de disc.
// Deixa els bytes llegits a bloc.
// Actualitza midaBloc amb el nombre de bytes llegits.
// Retorna false si troba cap EOF.
// Altrament, retorna true.
bool DescompressorPrac::llegeixBlocDeDisc(){

    std::vector <int> arbreLong (ELEMENTS_HUFFMAN, 0), bl_count (ELEMENTS_HUFFMAN, 0);
    std::vector <unsigned long> properCodi (ELEMENTS_HUFFMAN, 0);
    std::map <std::pair<unsigned long, int>, BinTree<int>> mapaCodi;
    BinTree<int> arbreIterar;

    // arbreLong := longitud de codi de cada byte <--
    // bl_count := nombre de bytes amb codi mida N
    // mapaCodi := codi real de cada byte <--
    // properCodi := variable auxiliar
    // arbreIterar := variable on desem l'arrel de l'arbre

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

    // Per a cada símbol, li asignem el seu codi de forma numèrica
    // i el desem a un mapa ordenat per longitud de codi, seguit del propi codi.
    // codi = codi mínim per a la longitud del codi del símbol
    //        + nombre de cops que s'ha asignat ja un codi d'aqusta mida
    for (int i = 0; i < ELEMENTS_HUFFMAN; ++i){
        int longit = arbreLong[i];
        if (longit){
            mapaCodi[std::pair<int, unsigned long>(longit, properCodi[longit])] = BinTree<int> (i);
            properCodi[longit]++;
        }
    }
    
    // Generació de l'arbre de Huffman
    // Agrupem de dos en dos sota un mateix pare els dos últims elements del mapa,
    // i afegim el pare de nou al mapa.
    // Com que el mapa s'ordena sol, quan resti un sol element, aquest serà l'arrel
    // de l'arbre sencer.
    while(mapaCodi.size() > 1){
        auto it = mapaCodi.end();
        --it;
        BinTree aux = it->second;
        mapaCodi.erase(it);
        it = mapaCodi.end();
        --it;
        mapaCodi[std::pair<int, unsigned long>(it->first.first - 1, it->first.second >> 1)] = BinTree(-1, it->second, aux);
        mapaCodi.erase(it);
    }
    
    // Llegim bits un a un de disc i travessem l'arbre reiteradament mentre emplenem el bloc.
    // Iterem indefinidament fins que trobem l'element de fi de bloc.
    arbreIterar = mapaCodi.begin()->second;
    midaBloc = 0;
    while (1){
        if (arbreIterar.value() == ELEMENTS_HUFFMAN - 1) return true;
        else if (arbreIterar.value() == -1){
            llegeixBitDeDisc() ? arbreIterar = arbreIterar.right() : arbreIterar = arbreIterar.left();
        }
        else{
            bloc[midaBloc++] = arbreIterar.value();
            arbreIterar = mapaCodi.begin()->second;
        }
    }
}

// Actualitza byteActualBloc amb el següent byte al bloc
// en cas de ser buit, demana el proper bloc.
// Retorna false si troba EOF. Altrament, retorna true.
bool DescompressorPrac::llegeixByteDelBloc(){
    if (posActualBloc == midaBloc){
        if (llegeixBlocDeDisc()){
            posActualBloc = 0;
        }
        else return false;
    }
    byteActualBloc = bloc[posActualBloc++];
    return true;
}

// Retorna el proper bit del bloc.
// Retorna -1 si troba EOF. Altrament, retorna el bit com a 1 o 0.
int DescompressorPrac::llegeixBitDelBloc(){
    if (bitsActualsBloc == 0){
        if (llegeixByteDelBloc()){
            bitsActualsBloc = 8;
        }
        else return -1;
    }
    return (byteActualBloc >> (--bitsActualsBloc)) & 1;
}

// Retorna els quantitat propers bits del bloc
// Comportament no definit per a quantitat > (sizeof(unsigned long) * 8).
// fiDeFitxer indica si s'ha trobat EOF.
unsigned long DescompressorPrac::llegeixBitsDelBloc(int quantitat, bool & fiDeFitxer){
    unsigned long temporal = 0;
    while (quantitat--){
        int nextBit = llegeixBitDelBloc();
        if (nextBit < 0){
            fiDeFitxer = true;
            return 0;
        }
        else {
            temporal |= nextBit << quantitat;
        }
    }
    fiDeFitxer = false;
    return temporal;
}


// Funció principal del descompressor.
// Rep dos camins a dos fitxers, entrada i sortida,
// i descomprimeix el fitxer que hi ha a paramEntrada
// i el desa a paramSortida.
void DescompressorPrac::descomprimeix (char * paramEntrada, char * paramSortida){

    // Comprovació dels paràmetres d'entrada.

    if (strcmp(paramEntrada, paramSortida) == 0){
        std::cerr << "Mateix argument d'entrada que de sortida." << std::endl;
        exit(1);
    }

    // Preparem fitxer d'entrada.
    fitxerEntrada = fopen (paramEntrada, "r");

    if (!fitxerEntrada){
        perror("Error obrint fitxer d'entrada");
        exit(1);
    }

    // Preparem fitxer d'escriptura.
    fitxerSortida = std::ofstream(paramSortida);

    if (!fitxerSortida){
        perror("Error obrint fitxer de sortida");
        exit(1);
    }

    // Iniciem algunes variables globals a zero.
    byteActualBloc = 0;
    byteActualDisc = 0;
    bitsActualsBloc = 0;
    bitsActualsDisc = 0;
    
    bool primersSimbolLinia = true;
    std::optional<long> anteriorNatural; // Número que vam descomprimir a l'anterior iteració del bucle.

    for (int res = llegeixBitDelBloc(); res >= 0; res = llegeixBitDelBloc()){
        if (res == 1){
            //0b1
            res = llegeixBitDelBloc();
            if (res < 0) break;
            if (res == 0){
                // mateix valor que abans
                // 0b10
                if (!primersSimbolLinia) fitxerSortida << " ";
                primersSimbolLinia = false;
                fitxerSortida << anteriorNatural.value();
            }
            else{
                // 0b11
                res = llegeixBitDelBloc();
                if (res < 0) break;
                if (res == 0){
                    // 0b110
                    res = llegeixBitDelBloc();
                    if (res < 0) break;
                    if (res == 0){
                        // Literal
                        // 0b1100
                        bool fiDeFitxer;
                        anteriorNatural = llegeixBitsDelBloc(31, fiDeFitxer);
                        if (fiDeFitxer) break;
                        if (!primersSimbolLinia) fitxerSortida << " ";
                        primersSimbolLinia = false;
                        fitxerSortida << anteriorNatural.value();
                    }
                    else{
                        // EOL
                        // 0b1101
                        fitxerSortida << std::endl;
                        primersSimbolLinia = true;
                    }
                }
                else{
                    // -1's
                    // 0b111
                    bool fiDeFitxer;
                    unsigned long iteracions = llegeixBitsDelBloc(13, fiDeFitxer);
                    if (fiDeFitxer) break;
                    if (!primersSimbolLinia) fitxerSortida << " ";
                    fitxerSortida << "-1";
                    while (iteracions--) fitxerSortida << " -1";
                    primersSimbolLinia = false;
                }
            }
        }
        else{
            // increment/decrement
            // 0b0
            long increment, bitsALlegir = 4, suma = 1;
            bool fiDeFitxer, signe;
            // per defecte, asumim que hem de llegir 4 bits pel valor absolut
            // i haurem de sumar 1 al valor absolut. Si el següent bit és 1,
            // vol dir que hem de llegir 8 o 12, i la suma serà 17 o 273,
            // depenent de si el següent a aquest és 0 o 1.
            res = llegeixBitDelBloc();
            if (res < 0) break;
            if (res > 0){
                res = llegeixBitDelBloc();
                if (res < 0) break;
                if (res == 0) {
                    bitsALlegir = 8;
                    suma = 17;
                }
                else {
                    bitsALlegir = 12;
                    suma = 273;
                }
            }
            // llegim el signe de la diferència
            res = llegeixBitDelBloc();
            if (res < 0) break;
            signe = res;
            increment = llegeixBitsDelBloc(bitsALlegir, fiDeFitxer);
            if (fiDeFitxer) break;
            if (!primersSimbolLinia) fitxerSortida << " ";
            primersSimbolLinia = false;
            increment += suma;
            if (signe) increment *= -1;
            anteriorNatural.value() += increment;
            fitxerSortida << anteriorNatural.value();
        }
    }
    // EOF

}

#endif