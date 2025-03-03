#include <iostream>
#include <string>
#include <vector>

#define parStruct std::pair <char, int>
#define vecStruct std::vector <parStruct>

int pow2roundup (int x) {
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

vecStruct recalcularCotes(vecStruct vin, int min, int max, int sumaGlobal){
    vecStruct nou (vin.size() + 1);
    int sumatori = 0;
    for (int i = 0; i < vin.size(); ++i){
        nou[i] = parStruct (vin[i].first, ((sumaGlobal - sumatori) * min + sumatori * max)/sumaGlobal);
        sumatori += vin[i].second;
    }
    nou[nou.size() - 1] = parStruct(NULL,max);
    return nou;
}

int main(){
    std::string missatge, alfabet, frequencies;
    int longitud;
    std::cout << "Escriu el missatge:" << std::endl;
    std::getline(std::cin,missatge);
    std::cout << "Escriu el l'alfabet:" << std::endl;
    std::getline(std::cin,alfabet);
    std::cout << "Escriu les freqüències:" << std::endl;
    std::getline(std::cin,frequencies);
    std::cout << "Escriu la longitud:" << std::endl;
    std::cin >> longitud;

    vecStruct decodificador;

    char caracter, actual, going;
    int total = 0;
    for (int i = 0, j = 0, num, fasei = 0, fasej; i < alfabet.size(); ++i){
        actual = alfabet[i];
        switch (fasei){
            case 0:
                if (actual == '\''){
                    ++fasei;
                }
                break;
            case 1:
                caracter = actual;
                //Bucle nombre
                for (num = 0, fasej = 0, going = 1 ; going; ++j){
                    actual = frequencies[j];
                    switch (fasej)
                    {
                    case 0:
                        if (actual >= '0' && actual <= '9'){
                            ++fasej;
                        }
                        else break;
                    case 1:
                        if (actual >= '0' && actual <= '9'){
                            num = num * 10 + actual - '0';
                        }
                        else{
                            going = 0;
                        }
                    }
                }
                decodificador.push_back(std::pair<char, int> (caracter, num));
                total += num;
                ++i;
                fasei = 0;
                break;
        }
    }

}