#include <string>
#include <string.h>
#include <iostream>
#include <optional>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <map>
#include "BinTree.hpp"

#define MAX_BLOC 32768
#define ELEMENTS_HUFFMAN 257

FILE * fitxerSortida; // Fitxer de sortida a disc.
u_int8_t byteActualBloc; // Byte codificat amn l'algorisme propietarique s'envia al Huffman.
u_int8_t byteActualDisc; // Byte codificat en huffman que enviem a disc.
int bitsActualsBloc; // Numero de bits escrits al byteActualBloc.
int bitsActualsDisc; // Numero de bits escrits al byteActualDisc.
u_int32_t midaBloc; // Numero de bytes enviats al bloc.
u_int8_t * bloc = new u_int8_t[MAX_BLOC]; // Bloc on desem els bits jacodificats de forma propietària abans de ser codificats amb Huffman.

void longitudArbre(const BinTree<__uint16_t> & arbre, std::vector <int> & longitudsCodi, int longitud){
    if (arbre.left().empty()){
        longitudsCodi[arbre.value()] = longitud;
    }
    else{
        longitudArbre(arbre.left(), longitudsCodi, longitud + 1);
        longitudArbre(arbre.right(), longitudsCodi, longitud + 1);
    }
}

void byteAlDisc(const u_int8_t nouByte){
    fwrite( & nouByte , sizeof(u_int8_t), 1, fitxerSortida);
}

void flushDisc(){
    if (bitsActualsDisc){
        byteActualDisc <<= (8 - bitsActualsDisc);
        byteAlDisc(byteActualDisc);
    }
    fclose(fitxerSortida);
}

void bitAlDisc(const bool bit){
    byteActualDisc <<= 1;
    ++bitsActualsDisc;
    if (bit) ++byteActualDisc;
    if (bitsActualsDisc == 8){
        byteAlDisc(byteActualDisc);
        byteActualDisc = 0;
        bitsActualsDisc = 0;
    }
}

void bitsAlDisc(unsigned long bits, int quantitat){
    while (quantitat--){
        bitAlDisc(bits & (1 << quantitat) );
    }
}

void blocAHuffman(){
    // Generar arbre per primer cop
    // L'arbre es genera amb el símbol adicional 257 que representa fi de bloc
    // L'arbre quedarà a iterador->second, trust me bro
    std::vector<int> iteracions (256, 0);
    for (int i = 0; i < midaBloc; ++i){
        iteracions[bloc[i]]++;
    }

    std::multimap<int, BinTree<u_int16_t> > insercions;
    for (int i = 0; i < 256; ++i){
        insercions.insert({iteracions[i], BinTree<__uint16_t> ((u_int16_t) i)});
    }
    // Afegim EOB, només ocorre un cop
    insercions.insert({1, BinTree<__uint16_t> (ELEMENTS_HUFFMAN - 1)});

    std::multimap<int, BinTree<u_int16_t> >::iterator iterador = insercions.begin();
    BinTree<u_int16_t> arbreTemp;

    while(insercions.size() > 1){
        int tempOcurr = iterador -> first;
        arbreTemp = iterador -> second;
        iterador = insercions.erase(iterador);
        insercions.insert({
            tempOcurr + iterador -> first,
            BinTree<__uint16_t>(-1, arbreTemp, iterador -> second)
        });
    iterador = insercions.erase(iterador);
    }

    // Obtenim les longituds de codi per poder refer l'arbre
    // amb les longituds "ordenades".

    std::vector <int> arbreLong (ELEMENTS_HUFFMAN, 0), bl_count (ELEMENTS_HUFFMAN, 0),
        arbreCodi (ELEMENTS_HUFFMAN, 0), properCodi (ELEMENTS_HUFFMAN, 0);

    // arbreLong := longitud de codi de cada byte <--
    // bl_count := nombre de bytes amb codi mida N
    // arbreCodi := codi real de cada byte <--
    // properCodi := variable auxiliar

    longitudArbre(iterador -> second, arbreLong, 0);

    int maxLong = 0, codi = 0;
    for (int i = 0; i < ELEMENTS_HUFFMAN; ++i){
        if (arbreLong[i] > maxLong) maxLong = arbreLong[i];
        bl_count[arbreLong[i]]++;
    }

    for (int i = 1; i <= maxLong; ++i){
        codi = (codi + bl_count[i - 1]) << 1;
        properCodi[i] = codi;
    }

    for (int i = 0; i < ELEMENTS_HUFFMAN; ++i){
        int longit = arbreLong[i];
        if (longit){
            arbreCodi[i] = properCodi[longit];
            properCodi[longit]++;
        }
    }

    // escriure les ELEMENTS_HUFFMAN mides de codi (per poder generar l'arbre després), segons arbreLong
    // -> per a cada simbol, si < 32; escrivim el literal amb 5 bits
    // -> si >= 32, escrivim 0b00000 seguit del literal amb 8 bits
    // escriure els elements del bloc codificats en huffman segons arbreCodi
    // escriure arbreCodi[256] (EOB)

    for (int i = 0; i < ELEMENTS_HUFFMAN; ++i){
        if (arbreLong[i] < 32){
            bitsAlDisc(arbreLong[i] & 0x1F, 5);
        }
        else{
            bitsAlDisc(0, 5);
            bitsAlDisc(arbreLong[i] & 0xFF, 8);
        }
    }
    for (int i = 0; i < midaBloc; ++i){
        bitsAlDisc(arbreCodi[bloc[i]], arbreLong[bloc[i]]);
    }
    bitsAlDisc(arbreCodi[bloc[ELEMENTS_HUFFMAN - 1]], arbreLong[bloc[ELEMENTS_HUFFMAN - 1]]);
}

void byteAlBloc(const u_int8_t nouByte, const bool forcaHuffman){
    bloc[midaBloc++] = nouByte;

    if (midaBloc == MAX_BLOC || forcaHuffman){
        blocAHuffman();
        midaBloc = 0;
    }
}

void bitAlBloc(const bool bit){
    byteActualBloc <<= 1;
    ++bitsActualsBloc;
    if (bit) ++byteActualBloc;
    if (bitsActualsBloc == 8){
        byteAlBloc(byteActualBloc, false);
        byteActualBloc = 0;
        bitsActualsBloc = 0;
    }
}

void bitsAlBloc(unsigned long bits, int quantitat){
    while (quantitat--){
        bitAlBloc(bits & (1 << quantitat) );
    }
}

void negatiusAlBloc(int & quantitat){
    if (!quantitat) return;
    bitsAlBloc(0b111, 3);
    bitsAlBloc(quantitat, 10);
    quantitat = 0;
}

void eolAlBloc(){
    bitsAlBloc(0b1101, 4);
}

void literalAlBloc(const long literal){
    bitsAlBloc(0b1100, 4);
    bitsAlBloc(literal, 31);
}

void diferenciaAlBloc(const long nou, const long antic){
    long diferencia = nou - antic;
    if (diferencia == 0){
        bitsAlBloc(0b10, 2);
        return;
    }
    long difAbs = abs(diferencia);
    bool signe = diferencia < 0;
    if (difAbs <= 16){
        bitsAlBloc(0b00, 2);
        bitAlBloc(signe);
        bitsAlBloc(difAbs - 1, 4);
    }
    else if(difAbs <= 272){
        bitsAlBloc(0b010, 3);
        bitAlBloc(signe);
        bitsAlBloc(difAbs - 17, 8);
    }
    else if(difAbs <= 4368){
        bitsAlBloc(0b011, 3);
        bitAlBloc(signe);
        bitsAlBloc(difAbs - 273, 12);
    }
    else{
        literalAlBloc(nou);
    }
}

void enfOfFile(){
    if (bitsActualsBloc){
        byteActualBloc <<= (8 - bitsActualsBloc);
        byteAlBloc(byteActualBloc, true);
    }
    bitsAlDisc(0, 13);
    flushDisc();
}



long properNumero(std::string const& entrada, long & index){
    long numActual = 0;
    bool negatiu = false;
    while (!((entrada[index] >= '0' && entrada[index] <= '9') || entrada[index] == '-')) ++index;
    if (entrada[index] == '-'){
        negatiu = true;
        ++index;
    }
    while (index < entrada.length() && entrada[index] >= '0' && entrada[index] <= '9'){
        numActual *= 10;
        numActual += entrada[index] - '0';
        ++index;
    }
    if (index == entrada.length()) index = -1;
    if (negatiu) numActual *= -1;
    return numActual;
}

/*
    Algorisme de compresió, fase 1 (potser després li faig un huffman).
    └─ Llegim número.
       ├─ És -1.
       │  └─ Prenem compte de quants -1 hem trobat seguits (fins a 1024).
       │     └─ Escrivim 0b111 + el número de -1 trobats, amb 10 bits.
       ├─ És fi de línia.
       │  ├─ Escrivim 0b1101.
       │  └─ Reiniciem a 0 NOMÉS el compador de -1 trobats.
       └─ És un enter positiu.
          ├─ És el primer que trobem.
          │  └─ Escrivim 0b1100 + el literal sense signe amb 31 bits.
          └─ Ja hem trobat un natural abans.
             └─ Calculem la diferència entre l'anterior i el nou natural:
                ├─ Són iguals.
                │  └─ Escrivim 0b10.
                ├─ 1 <= Diferència <= 16:
                │  └─ Escrivim 0b00 + la diferència amb 5 bits*. ───┐
                ├─ 17 <= Diferència <= 272:                         │
                │  └─ Escrivim 0b010 + la diferència amb 9 bits*. ──┤
                ├─ 273 <= Diferència <= 4368:                       │
                │  └─ Escrivim 0b011 + la diferència amb 13 bits*. ─┤
                │       Les diferències s'escriuen: ────────────────┘
                │       ├─ 1 bit per signe (0 positiu, 1 negatiu).
                │       └─ x bit pel valor absolut - llindar de diferència.
                └─ 4368 < Diferència:
                   └─ Escrivim 0b1100 + el literal sense signe amb 31 bits.
*/

int main (int argc, char ** argv){

    if (argc != 3){
        std::string misError (argv[0]);
        std::cerr << "Arguments incorrectes.\nUs: " + misError + " entrada.txt sortida.knk" << std::endl;
        exit(1);
    }

    if (strcmp(argv[1], argv[2]) == 0){
        std::cerr << "Mateix argument d'entrada que de sortida." << std::endl;
        exit(1);
    }

    std::ifstream fitxerEntrada(argv[1]);

    if (!fitxerEntrada){
        perror("Error obrint fitxer d'entrada");
        exit(1);
    }

    fitxerSortida = fopen (argv[2], "w");

    if (!fitxerSortida){
        perror("Error obrint fitxer de sortida");
        exit(1);
    }

    byteActualBloc = 0;
    byteActualDisc = 0;
    bitsActualsBloc = 0;
    bitsActualsDisc = 0;
    midaBloc = 0;

    std::string entrada;
    long numActual, index;
    int comptadorNegs;
    std::optional <long> anteriorNatural;
    while (std::getline(fitxerEntrada, entrada)){
        // Iterem per cada línia

        index = 0;
        comptadorNegs = 0;

        while (index > -1){
            numActual = properNumero(entrada, index);
            if (numActual == -1){
                // Trobem un -1.
                // Només incrementem comptador.
                ++comptadorNegs;

                if (comptadorNegs == 0b1111111111) {
                    negatiusAlBloc(comptadorNegs);
                }
            }
            else{
                // Trobem un natural

                // Escrivim la quantitat de negatius que
                // haviem trobat fins ara.
                negatiusAlBloc(comptadorNegs);

                if (anteriorNatural){
                    // Ja hem trobat un natural abans.
                    diferenciaAlBloc(numActual, anteriorNatural.value());
                }
                else{
                    // Primer cop que trobem un natural.
                    literalAlBloc(numActual);
                }
                anteriorNatural = numActual;
            }
        }
        negatiusAlBloc(comptadorNegs);

        //EOL
        eolAlBloc();
    }
    //EOF
    enfOfFile();
}