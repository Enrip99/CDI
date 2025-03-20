#include <iostream>
#include <string>
#include <vector>

#define MATCHLENGTH 4
#define OFFSET 12

int main(){
    std::string entrada;
    std::getline(std::cin, entrada);

    int index = 0;
    int midaBandera = 0;
    u_int8_t banderes = 0; // bit menys pes el primer que es llegeix

    /*
    // Imprimeix 'á'
    std::vector <char> vec = { (char) 0b11000011, (char) 0b10100001};
    std::string s (vec.begin(), vec.end());
    std::cout << s << std::endl;
    */

    std::vector <u_int8_t> decodifica;
    
    while (index < entrada.length()){
        if (midaBandera--){
            if (banderes & ( 1 << ( 7 - midaBandera ))){
                // Referència
                int matLen = 0, offset = 0;
                for (int i = 0; i < MATCHLENGTH ; ++i){
                    if (entrada[index+i] == '1'){
                        matLen += 1 << ( ( MATCHLENGTH - 1 ) - i);
                    }
                }
                ++matLen;
                index += MATCHLENGTH ;
                for (int i = 0; i < OFFSET ; ++i){
                    if (entrada[index+i] == '1'){
                        offset += 1 << ( ( OFFSET - 1 ) - i);
                    }
                }
                ++offset;
                index += OFFSET ;
                while (matLen--){
                    decodifica.push_back(decodifica[decodifica.size() - offset]);
                }
            }
            else{
                // Literal
                u_int8_t byteActual = 0;
                for (int i = 0; i < 8; ++i){
                    if (entrada[index+i] == '1'){
                        byteActual += 1 << (7 - i);
                    }
                }
                decodifica.push_back(byteActual);
                index += 8;
            }
        }
        else {
            banderes = 0;
            for (int i = 0; i < 8; ++i){
                if (entrada[index+i] == '1'){
                    banderes += 1 << i;
                }
            }
            index += 8;
            midaBandera = 8;
        }
    }

    std::string s (decodifica.begin(), decodifica.end());
    std::cout << s << std::endl;
    
}