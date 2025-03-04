#include <iostream>
#include <string>
#include <vector>

#define parStruct std::pair <char, int>
#define vecStruct std::vector <parStruct>

int cercaBinaria (vecStruct & vec, int primer, int ultim, char objectiu){
    int mig = (primer + ultim) / 2;
    if (objectiu > vec[mig].first) return cercaBinaria(vec, mig+1, ultim, objectiu);
    if (objectiu < vec[mig].first) return cercaBinaria(vec, primer, mig-1, objectiu);
    return mig;
}

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

    int cotaMax = pow2roundup(total * 4);
    int index = cotaMax, bitsPerNum = 0;
    while (index >>= 1) ++bitsPerNum;

    //vecStruct vectorActual = recalcularCotes(decodificador, 0, cotaMax, total);
    vecStruct vectorActual;
    int cotaMinAct = 0, cotaMaxAct = cotaMax;

    int quart = cotaMax/4, meitat = cotaMax/2, tresquart = quart + meitat;
    int i = 0, holds = 0;
    bool  done = false;
    std::string codi;

    while (not done){
        if (cotaMaxAct <= meitat){
            //E1
            cotaMinAct = cotaMinAct * 2;
            cotaMaxAct = cotaMaxAct * 2;
            codi += '0';
            while (holds){
                codi += '1';
                --holds;
            }
        }
        else if (cotaMinAct >= meitat){
            //E2
            cotaMinAct = cotaMinAct * 2 - cotaMax;
            cotaMaxAct = cotaMaxAct * 2 - cotaMax;
            codi += '1';
            while (holds){
                codi += '0';
                --holds;
            }
        }
        else if (cotaMinAct >= quart && cotaMaxAct <= tresquart){
            //E3
            cotaMinAct = cotaMinAct * 2 - meitat;
            cotaMaxAct = cotaMaxAct * 2 - meitat;
            ++holds;
        }
        else{
            if (i == longitud){
                //FI
                if (cotaMinAct <= quart){
                    codi += "01";
                    while (holds){
                        codi += '1';
                        --holds;
                    }
                }
                else{
                    codi += "10";
                    while (holds){
                        codi += '0';
                        --holds;
                    }
                }
                done = true;
                while (cotaMax >>= 1){
                    if (cotaMinAct & cotaMax) codi += '1';
                    else codi += '0';
                }
            }
            else{
                //HIT
                vectorActual = recalcularCotes(decodificador, cotaMinAct, cotaMaxAct, total);
                //....
                int pos = cercaBinaria(vectorActual, 0, vectorActual.size() - 1, missatge[i]);
                cotaMinAct = vectorActual[pos].second;
                cotaMaxAct = vectorActual[pos+1].second;
                ++i;

            }
        }
    }
    std::cout << codi << std::endl;

}