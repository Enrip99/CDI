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
uint8_t byteActualBloc; // Byte codificat amn l'algorisme propietarique s'envia al Huffman.
uint8_t byteActualDisc; // Byte codificat en huffman que enviem a disc.
int bitsActualsBloc; // Nombre de bits escrits al byteActualBloc.
int bitsActualsDisc; // Nombre de bits escrits al byteActualDisc.
uint32_t midaBloc; // Nombre de bytes enviats al bloc.
uint8_t * bloc = new uint8_t[MAX_BLOC]; // Bloc on desem els bits jacodificats de forma propietària abans de ser codificats amb Huffman.

// Escriu nouByte a disc.
void byteAlDisc(const uint8_t nouByte){
    fwrite( & nouByte , sizeof(uint8_t), 1, fitxerSortida);
}

// Termina d'escriure a disc qualsevol bit que encara no s'haguès escrit
// en un octet sencer i tanca la sortida.
void flushDisc(){
    if (bitsActualsDisc){
        byteActualDisc <<= (8 - bitsActualsDisc);
        byteAlDisc(byteActualDisc);
    }
    fclose(fitxerSortida);
}

// Demana escriure un bit a disc com a nou bit de menys pes.
//
// Internament, ho afegeix a byteActualDisc, i incrementa bitsActualsDisc.
// Cada 8 iteracions, s'envia byteActualDisc al disc, i reinicialitza
// byteActualDisc i bitsActualsDisc a 0.
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

// Demana escriure a disc quantitat bits de menys pes de bits.
// S'envien de bit de més pes a bit de menys pes.
// Funcionament no definit per a quantitat > (sizeof(unsigned long) * 8).
void bitsAlDisc(unsigned long bits, int quantitat){
    while (quantitat--){
        bitAlDisc(bits & (1 << quantitat) );
    }
}

/*
Estructura de la codificació Huffman:
│
└─ Genera un primer arbre fent servir el nombre de cops
    que apareix cada byte al bloc.
    L'arbre conté una entrada per cada byte 0x00..0xFF i
    una entrada (257) pel símbol de fi de bloc (EOB).
    │
    └─ Recalculem els codis de cada element en funció de
       la longitud del codi i el propi valor del símbol.
       Descartem els codis de l'anterior pas.
       (Re: RFC 1951 3.2.2. Use of Huffman coding in the "deflate" format).
       │
       └─ Escrivim els codis de Huffman de forma literal.
          La mida en bits del codi depèn de la seva mida.
          │ │
          │ ├─ Per a mides < 32: escrivim la mida amb 5 bits.
          │ │
          │ ├─ Per a mides >= 32: escrivim 0b00000 seguit de la mida amb
          │ │  8 bits de forma literal. Els valors 1..31 no es fan servir.
          │ │
          │ └─ El valor 0b0000000000000 (13 bits a 0) indiquen fi del fitxer (EOF).
          │    Només ha d'aparèixer com a primer element d'una "sèrie de codis".
          │
          └─ A continuació s'envien tots els símbols del bloc, codificats
             amb el seu codi de Huffman corresponent.
             │
             └─ Per acabar, s'envia el codi per a fi de fitxer (veure més amunt).

Un bloc codificat en huffman conprèn una quantitat indeterminada de bits.
Noteu que un bloc de Huffman està basat en bits, i no pas bytes. Un bloc pot començar
o acabar a qualsevol posició dins un byte, no està confinat a ocupar un nombre enter de bytes. 
*/


// Funció interna recursiva de longitudArbre.
void longitudArbreIntern(const BinTree<uint16_t> & arbre, std::vector <int> & longitudsCodi, int longitud){
    if (arbre.left().empty()){
        longitudsCodi[arbre.value()] = longitud;
    }
    else{
        longitudArbreIntern(arbre.left(), longitudsCodi, longitud + 1);
        longitudArbreIntern(arbre.right(), longitudsCodi, longitud + 1);
    }
}

// Funció que calcula la longitud del codi per a tots els elements de arbre, i les introdueix a longitudsCodi.
// Requereix que longitudsCodi tingui com a mínim longitud >= nombre de fulles de arbre.
void longitudArbre(const BinTree<uint16_t> & arbre, std::vector <int> & longitudsCodi){
    longitudArbreIntern(arbre, longitudsCodi, 0);
}

// Genera els codis de Huffman pertinents al bloc i escriu a disc el bloc codificat.
void blocAHuffman(){
    // Generar arbre per primer cop
    // L'arbre es genera amb el símbol adicional 257 que representa fi de bloc.
    // L'arbre quedarà a iterador->second.

    // Primer, recollim el nombre de cops que ha aparegut cada símbol al bloc.
    std::vector<int> iteracions (256, 0);
    for (int i = 0; i < midaBloc; ++i){
        iteracions[bloc[i]]++;
    }

    // Creem un mapa amb una fulla per cada símbol, ordenat pel nombre de repeticions.
    std::multimap<int, BinTree<uint16_t> > insercions;
    for (int i = 0; i < 256; ++i){
        insercions.insert({iteracions[i], BinTree<uint16_t> ((uint16_t) i)});
    }
    // Afegim EOB, només ocorre un cop
    insercions.insert({1, BinTree<uint16_t> (ELEMENTS_HUFFMAN - 1)});

    std::multimap<int, BinTree<uint16_t> >::iterator iterador = insercions.begin();
    BinTree<uint16_t> arbreTemp;

    // Fins quedar-nos amb un sol node, iterem pel mapa agafant els dos elements
    // de menor pes, i juntant-los en un de nou amb nodeNou.pes = nodeA.pes + nodeB.pes,
    // essent els fills de nouNode, nodeA i nodeB.
    while(insercions.size() > 1){
        int tempOcurr = iterador -> first;
        arbreTemp = iterador -> second;
        iterador = insercions.erase(iterador);
        insercions.insert({
            tempOcurr + iterador -> first,
            BinTree<uint16_t>(-1, arbreTemp, iterador -> second)
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

    // Obtenim les longituds dels codis per a cada símbol.
    longitudArbre(iterador -> second, arbreLong);

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
            arbreCodi[i] = properCodi[longit];
            properCodi[longit]++;
        }
    }

    // Escrivim en ordre les mides de tots els codi,
    // ├─ si mida < 32, escrivim el literal amb 5 bits.
    // └─ si mida >= 32, escrivim 0b00000 seguit del literal amb 8 bits.
    for (int i = 0; i < ELEMENTS_HUFFMAN; ++i){
        if (arbreLong[i] < 32){
            bitsAlDisc(arbreLong[i] & 0x1F, 5);
        }
        else{
            bitsAlDisc(0, 5);
            bitsAlDisc(arbreLong[i] & 0xFF, 8);
        }
    }

    // Escrivim els elements del bloc codificats en huffman.
    for (int i = 0; i < midaBloc; ++i){
        bitsAlDisc(arbreCodi[bloc[i]], arbreLong[bloc[i]]);
    }

    // Escrivim el codi pel signe de final de bloc (257).
    bitsAlDisc(arbreCodi[bloc[ELEMENTS_HUFFMAN - 1]], arbreLong[bloc[ELEMENTS_HUFFMAN - 1]]);
}

// Afegeix nouByte al bloc.
// Cada cop que s'omple el bloc, o forcaHuffman és true,
// es codifica el bloc amb huffman, s'escriu a disc i es buida el bloc.
void byteAlBloc(const uint8_t nouByte, const bool forcaHuffman){
    bloc[midaBloc++] = nouByte;

    if (midaBloc == MAX_BLOC || forcaHuffman){
        blocAHuffman();
        midaBloc = 0;
    }
}

// "Afegeix" un bit al bloc com a nou bit de menys pes.
//
// Internament, ho afegeix a byteActualBloc, i incrementa bitsActualsBloc.
// Cada 8 iteracions, s'envia byteActualBloc al bloc, i reinicialitza
// byteActualBloc i bitsActualsBloc a 0.
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

// Envia al bloc els quantitat bits de menys pes de bits.
// S'envien de bit de més pes a bit de menys pes.
// Funcionament no definit per a quantitat > (sizeof(unsigned long) * 8).
void bitsAlBloc(unsigned long bits, int quantitat){
    while (quantitat--){
        bitAlBloc(bits & (1 << quantitat) );
    }
}

// Envia al bloc el nombre de negatius seguits trobats.
// Si quantitat és 0, no fa res.
// S'envia 0b111 + (quantitat - 1) (10 bits).
// S'actualitza el valor de quantita a 0. 
void negatiusAlBloc(int & quantitat){
    if (!quantitat) return;
    bitsAlBloc(0b111, 3);
    bitsAlBloc(quantitat - 1, 10);
    quantitat = 0;
}

// Envia al bloc el codi de fi de línia (EOL).
// S'envia 0b1101.
void eolAlBloc(){
    bitsAlBloc(0b1101, 4);
}

// Envia al bloc un número natural.
// S'envia 0b1100 + literal (31 bits).
void literalAlBloc(const long literal){
    bitsAlBloc(0b1100, 4);
    bitsAlBloc(literal, 31);
}

// Envia al bloc la diferència entre nou i antic...
// En cas de ser iguals, envia 0b10.
// En cas de diferir...
// - per menys de 17: 0b00 + dif (5 bits).
// - per menys de 273: 0b010 + dif (9 bits).
// - per menys de 4269: 0b011 + dif (13 bits).
//  - les diferències s'envien en format Signe-Magnitud.
//
// En cas de diferir per 4269 o més, s'escriu el literal.
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

// Obliga a escriure al bloc els bits restants a bitsActualsBloc,
// força la compressió huffman i escriptura del bloc actual,
// escriu a disc els 13 bits a 0 corresponent a un EOF,
// i finalment mana escriure el búfer a disc i tancar el fitxer.
void endOfFile(){
    if (bitsActualsBloc){
        byteActualBloc <<= (8 - bitsActualsBloc);
        byteAlBloc(byteActualBloc, true);
    }
    bitsAlDisc(0, 13);
    flushDisc();
}


// Itera pel string entrada, a partir de la posició index fins que
// troba un número natural o un -1.
// Retorna el número trobat, i actualitza index apuntant després d'aquest.
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
Algorisme de compresió, fase 1 (després rep un huffman).
└─ Llegim número.
   ├─ És -1.
   │  └─ Prenem compte de quants -1 hem trobat seguits (fins a 1024).
   │     └─ Escrivim 0b111 + el número de "-1" seguits trobats menys u; amb 10 bits.
   │
   ├─ És fi de línia.
   │  ├─ Escrivim 0b1101.
   │  └─ Reiniciem a 0 NOMÉS el compador de -1 trobats.
   │
   └─ És un enter positiu.
      ├─ És el primer que trobem.
      │  └─ Escrivim 0b1100 + el literal sense signe amb 31 bits.
      │
      └─ Ja hem trobat un natural abans.
         └─ Calculem la diferència entre l'anterior i el nou natural:
            ├─ Són iguals.
            │  └─ Escrivim 0b10.
            │
            ├─ 1 <= Diferència <= 16:
            │  └─ Escrivim 0b00 + la diferència amb 5 bits*. ───┐
            │                                                   │
            ├─ 17 <= Diferència <= 272:                         │
            │  └─ Escrivim 0b010 + la diferència amb 9 bits*. ──┤
            │                                                   │
            ├─ 273 <= Diferència <= 4368:                       │
            │  └─ Escrivim 0b011 + la diferència amb 13 bits*. ─┤
            │                                                   │
            │       Les diferències s'escriuen: ────────────────┘
            │       ├─ 1 bit per signe (0 positiu, 1 negatiu).
            │       └─ x bit pel valor absolut - llindar de diferència.
            │
            └─ 4368 < Diferència:
               └─ Escrivim 0b1100 + el literal sense signe amb 31 bits.
*/

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
    std::ifstream fitxerEntrada(argv[1]);

    if (!fitxerEntrada){
        perror("Error obrint fitxer d'entrada");
        exit(1);
    }

    // Preparem fitxer d'escriptura.
    fitxerSortida = fopen (argv[2], "w");

    if (!fitxerSortida){
        perror("Error obrint fitxer de sortida");
        exit(1);
    }

    // Iniciem algunes variables globals a zero.
    byteActualBloc = 0;
    byteActualDisc = 0;
    bitsActualsBloc = 0;
    bitsActualsDisc = 0;
    midaBloc = 0;

    std::string entrada; // Cadena on llegim cada linia del fitxer d'entrada.
    long numActual; // Número que llegim de l'entrada i que comprimirem a cada iteració.
    long index; // Iterador sobre la cadena d'entrada.
    int comptadorNegs; // Número de "-1" seguits que hem llegit.
    std::optional <long> anteriorNatural; // Número que vam llegir a l'anterior iteració del bucle.

    while (std::getline(fitxerEntrada, entrada)){
        // Iterem per cada línia

        index = 0;
        comptadorNegs = 0;

        while (index > -1){
            // Iterem per tots els caracters de la línia

            numActual = properNumero(entrada, index);

            if (numActual == -1){
                // Trobem un -1.
                // Només incrementem comptador.
                ++comptadorNegs;

                if (comptadorNegs == 1024) {
                    // En cas de portar 1024 negatius seguits, els escrivim
                    // i tornem a posar el comptador a 0.
                    // Això es deu a que els -1 es comuniquen amb 10 bits.
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
                // Posem el "número de l'anterior cicle" per la propera iteració.
                anteriorNatural = numActual;
            }
        }
        // Com que ha acabat la línia, escrivim els -1 que teníem a la recàmara.
        negatiusAlBloc(comptadorNegs);

        // I finalment enviem el símbol de final de línia.
        eolAlBloc();
    }
    
    // Escriu EOF a disc, i finalitza les escriptures del bloc a disc.
    endOfFile();
}