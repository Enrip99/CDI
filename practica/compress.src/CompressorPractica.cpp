#include <string>
#include <string.h>
#include <iostream>
#include <optional>
#include <fstream>
#include <stdio.h>

FILE * fitxerSortida;
u_int8_t byteActual;
int bitsActuals;

void escriuBit(bool bit){
    byteActual <<= 1;
    ++bitsActuals;
    if (bit) ++byteActual;
    if (bitsActuals == 8){
        fwrite( & byteActual , sizeof(u_int8_t), 1, fitxerSortida);
        byteActual = 0;
        bitsActuals = 0;
    } 
}

void escriuBits(unsigned long bits, int quantitat){
    while (quantitat--){
        escriuBit(bits & (1 << quantitat) );
    }
}

void escriuNegatius(int & quantitat){
    if (!quantitat) return;
    escriuBits(0b111, 3);
    escriuBits(quantitat, 10);
    quantitat = 0;
}

void escriuEOL(){
    escriuBits(0b1101, 4);
}

void escriuEOF(){
    escriuEOL();
    if (bitsActuals){
        byteActual <<= (8 - bitsActuals);
        fwrite( & byteActual , sizeof(u_int8_t), 1, fitxerSortida);
    }
    fclose(fitxerSortida);
}

void escriuLiteral(long literal){
    escriuBits(0b1100, 4);
    escriuBits(literal, 31);
}

void escriuDiferencia(long nou, long antic){
    long diferencia = nou - antic;
    if (diferencia == 0){
        escriuBits(0b10, 2);
        return;
    }
    long difAbs = abs(diferencia);
    bool signe = diferencia < 0;
    if (difAbs <= 16){
        escriuBits(0b00, 2);
        escriuBit(signe);
        escriuBits(difAbs - 1, 4);
    }
    else if(difAbs <= 272){
        escriuBits(0b010, 3);
        escriuBit(signe);
        escriuBits(difAbs - 17, 8);
    }
    else if(difAbs <= 4368){
        escriuBits(0b011, 3);
        escriuBit(signe);
        escriuBits(difAbs - 273, 12);
    }
    else{
        escriuLiteral(nou);
    }
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
       │  ├─ Reiniciem a 0 NOMÉS el compador de -1 trobats.
       │  └─ Fi del fitxer es representa amb dos EOL seguits (una linia buida al final).
       └─ És un enter positiu.
          ├─ És el primer que trobem.
          │  └─ Escrivim 0b1100 + el literal sense signe amb 31 bits.
          └─ Ja hem trobat un natural abans.
             └─ Calculem la diferència entre l'anterior i el nou natural:
                ├─ Són iguals.
                │  └─ Escrivim 0b10.
                ├─ 1 <= Diferència <= 16:
                │  └─ Escrivim 0b00 + la diferència amb 5 bits*. ──┐
                ├─ 17 <= Diferència <= 272:                        │
                │  └─ Escrivim 0b010 + la diferència amb 9 bits*. ──┤
                ├─ 273 <= Diferència <= 4368:                      │
                │  └─ Escrivim 0b011 + la diferència amb 13 bits*. ─┤
                │       Les diferències s'escriuen: ───────────────┘
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

    byteActual = 0;
    bitsActuals = 0;

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
                    escriuNegatius(comptadorNegs);
                }
            }
            else{
                // Trobem un natural

                // Escrivim la quantitat de negatius que
                // haviem trobat fins ara.
                escriuNegatius(comptadorNegs);

                if (anteriorNatural){
                    // Ja hem trobat un natural abans.
                    escriuDiferencia(numActual, anteriorNatural.value());
                }
                else{
                    // Primer cop que trobem un natural.
                    escriuLiteral(numActual);
                }
                anteriorNatural = numActual;
            }
        }
        escriuNegatius(comptadorNegs);

        //EOL
        escriuEOL();
    }
    //EOF
    escriuEOF();
}