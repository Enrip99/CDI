#include <iostream>
#include <string>
#include <vector>

int pow2roundup (int x) {
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}


int main(){
    std::string codi, alfabet, frequencies;
    int longitud;
    std::cout << "Escriu el codi:" << std::endl;
    std::getline(std::cin,codi);
    std::cout << "Escriu el l'alfabet:" << std::endl;
    std::getline(std::cin,alfabet);
    std::cout << "Escriu les freqüències:" << std::endl;
    std::getline(std::cin,frequencies);
    std::cout << "Escriu la longitud:" << std::endl;
    std::cin >> longitud;
    
    std::vector <std::pair <char, int>> decodificador;

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

    /*for (int i = 0; i < decodificador.size(); ++i){
        std::cout << decodificador[i].first << " - " << decodificador[i].second << std::endl;
    }*/

    int cotaMax = pow2roundup(total);
    int index = cotaMax, targetlevel = 0;
    while (index >>= 1) ++targetlevel;


}
