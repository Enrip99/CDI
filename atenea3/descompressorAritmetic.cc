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
        nou[i] = parStruct (vin[i].first, min + (sumatori * max) / sumaGlobal);
        sumatori += vin[i].second;
    }
    nou[nou.size() - 1] = parStruct(NULL,max);
    return nou;
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

    /*for (int i = 0; i < decodificador.size(); ++i){
        std::cout << decodificador[i].first << " - " << decodificador[i].second << std::endl;
    }*/

    int cotaMax = pow2roundup(total * 4);
    int index = cotaMax, bitsPerNum = 0;
    while (index >>= 1) ++bitsPerNum;

    vecStruct vectorActual = recalcularCotes(decodificador, 0, cotaMax, total);
    int cotaMinAct = 0, cotaMaxAct = cotaMax;

    for (int i = 0; i < vectorActual.size(); ++i){
        std::cout << vectorActual[i].first << " - " << vectorActual[i].second << std::endl;
    }

    int fraccio = 0, i = 0;
    for (; i < bitsPerNum; ++i){
        fraccio *= 2;
        if (codi[i] == '1'){
            ++fraccio;
        }
    }

    int quart = cotaMax/4, meitat = cotaMax/2, tresquart = quart + meitat;


    std::string missatge;
    std::cout << "PRE: i: " << i << " -fraccio: " << fraccio << " - cotaminact: " << cotaMinAct << " - cotamaxact: " << cotaMaxAct << std::endl;

    while (i < codi.size()){
    //    if (fraccio < 5000 && fraccio > -1)std::cout << fraccio << std::endl;
        if (cotaMaxAct < meitat){
            //E0
            cotaMinAct = cotaMinAct * 2;
            cotaMaxAct = cotaMaxAct * 2;
            fraccio = fraccio * 2;
            if (codi[i] == '1') ++fraccio;
            ++i;
            std::cout << "E0: i: " << i << " -fraccio: " << fraccio << " - cotaminact: " << cotaMinAct << " - cotamaxact: " << cotaMaxAct << std::endl;
        }
        else if (cotaMinAct > meitat){
            //E1
            cotaMinAct = cotaMinAct * 2 - cotaMax;
            cotaMaxAct = cotaMaxAct * 2 - cotaMax;
            fraccio = fraccio * 2 - cotaMax;
            if (codi[i] == '1') ++fraccio;
            ++i;
            std::cout << "E1: i: " << i << " - fraccio: " << fraccio << " - cotaminact: " << cotaMinAct << " - cotamaxact: " << cotaMaxAct << std::endl;
        }
        else if (cotaMinAct > quart && cotaMaxAct < tresquart){
            //E2
            cotaMinAct = cotaMinAct * 2 - meitat;
            cotaMaxAct = cotaMaxAct * 2 - meitat;
            fraccio = fraccio * 2 - meitat;
            if (codi[i] == '1') ++fraccio;
            ++i;
            std::cout << "E2: i: " << i << " - fraccio: " << fraccio << " - cotaminact: " << cotaMinAct << " - cotamaxact: " << cotaMaxAct << std::endl;
        }
        else{
            vectorActual = recalcularCotes(decodificador, cotaMinAct, cotaMaxAct, total);
            //....
            int j = 0;
            for (; j < vectorActual.size() && fraccio < vectorActual[j+1].second; ++j);
            //std::cout << j << " - " << vectorActual[j].first << std::endl;
            missatge.push_back(vectorActual[j].first);
            cotaMinAct = vectorActual[j].second;
            cotaMaxAct = vectorActual[j+1].second;
            std::cout << "HIT: i: " << i << " - fraccio: " << fraccio << " - cotaminact: " << cotaMinAct << " - cotamaxact: " << cotaMaxAct << std::endl;
        }
    }
    std::cout << missatge.substr(0,longitud) << std::endl;
}
