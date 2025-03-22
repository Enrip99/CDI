#include <iostream>
#include <string>
#include <vector>

const int extraBitsLongVec[29] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
const int longVec[29] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
const int extraBitsDistVec[32] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,0,0};
const int distVec[32] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0};

int main(){
    std::string entrada;
    std::getline(std::cin, entrada);

    int index = 0;
    std::vector <u_int8_t> decodifica;
    //int fase = 0;
    while (index < entrada.length()){
        int primerNum = 0, codi;
        // llegim els primers 6 bits del codi codificat en huffman
        for (int i = 0; i < 6; ++i){
            primerNum <<= 1;
            if (entrada[index++] == '1') ++primerNum;
        }

        // llegim els 0..2 
        if (primerNum >= 0b110000 && primerNum <= 0b110001){
            // 280 - 287
            // necessita 8 bits
            // llegim dos més
            for (int i = 0; i < 2; ++i){
                primerNum <<= 1;
                if (entrada[index++] == '1') ++primerNum;
            }
            codi = primerNum + 88; // 280 - 0b11000000 (192)
        }
        else if (primerNum >= 0b000000 && primerNum <= 0b001011){
            // 256 - 279
            // 
            // necessita 7 bits
            // llegim un més
            primerNum <<= 1;
            if (entrada[index++] == '1') ++primerNum;
            codi = primerNum + 256;
        }
        else if (primerNum >= 0b001100 && primerNum <= 0b101111){
            // 0 - 143
            // necessita 8 bits
            // llegim dos més
            for (int i = 0; i < 2; ++i){
                primerNum <<= 1;
                if (entrada[index++] == '1') ++primerNum;
            }
            codi = primerNum - 0b00110000;
        }
        else if (primerNum >= 0b110010 and primerNum <= 0b1111111){
            // 144 - 255
            // necessita 9 bits
            // llegim tres més
            for (int i = 0; i < 3; ++i){
                primerNum <<= 1;
                if (entrada[index++] == '1') ++primerNum;
            }
            codi = primerNum - 256; // 0b110010000 (400) - 144 = 256
        }

        if (codi >= 0 && codi <= 255){
            // literal
            decodifica.push_back((u_int8_t) codi);
        }
        if (codi == 256){
            // fi de bloc
            // no fem res?
        }
        if (codi >= 257 && codi <= 285){
            // mirem enrere
            int longitud = 0, codiDist = 0, distancia = 0;
            // calculem quants haurem de copiar
            for (int i = 0; i < extraBitsLongVec[codi - 257]; ++i){
                longitud <<= 1;
                if (entrada[index++] == '1') ++longitud;
            }
            longitud += longVec[codi - 257];

            // calculem quant enrere
            for (int i = 0; i < 5; ++i){
                codiDist <<= 1;
                if (entrada[index++] == '1') ++codiDist;
            }
            for (int i = 0; i < extraBitsDistVec[codiDist]; ++i){
                distancia <<= 1;
                if (entrada[index++] == '1') ++distancia;
            }
            distancia += distVec[codiDist];

            for (int i = 0; i < longitud; ++i){
                decodifica.push_back(decodifica[decodifica.size() - distancia]);
            }
        }
    }
    
    std::string s (decodifica.begin(), decodifica.end());
    std::cout << s << std::endl;

}