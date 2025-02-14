#include <iostream>
#include <string>
#include <vector>
#include <bitset>

void usage(char *argv0){
    std::cout << "Usage: " << argv0 << " (-d|-c)" << std::endl;
    exit(1);
}

int main(int argc, char *argv[]){

    struct TRIPLET {
        char paraula;
        int longitud;
        int codi;
    };

    bool codifica;
    
    
    if (argc !=2) usage(argv[0]);
    std::string arg1 (argv[1]);
    if (arg1.compare("-d") == 0) codifica = false;
    else if (arg1.compare("-c") == 0) codifica = true;
    else usage(argv[0]);

    std::cout << "Escriu el codi del missatge:" << std::endl;

    std::string codificacioPla, texte;
    std::getline(std::cin,codificacioPla);

    if (codifica)   std::cout << "Escriu el texte que vols codificar:" << std::endl;
    else            std::cout << "Escriu el texte que vols decodificar:" << std::endl;
    std::getline(std::cin,texte);

    
    //Tots els codis van dins un vector que recorrem de forma lineal. Ineficient? Sí.
    //Però ho vaig fer sense pensar perque això haurà de córrer només 4 cops en tota sa vida.
    std::vector <TRIPLET> vecTriplets; 

    //passem la codificacio a un format amb que pugui treballar
    //sense detecció d'error. Ho sento profe. T'estimo molt <3
    int llargariaMax = 0;
    char caracter;
    for (int i = 0, fase = 0, num = 0;  i < codificacioPla.size(); ++i){
        char actual = codificacioPla[i];
        switch (fase){
            case 0:
                if (actual == '(') {
                    ++fase;
                    ++i;
                }
                break;
            case 1:
                caracter = actual;
                ++fase;
                break;
            case 2:
                if (actual >= '0' and actual <= '9'){
                    ++fase;
                }
                else break;
            case 3:
            if (actual >= '0' and actual <= '9'){
                num = num * 10 + actual - '0';
            }
            else if (actual == ')'){
                vecTriplets.push_back(TRIPLET {caracter, num});
                llargariaMax = std::max(llargariaMax, num);
                num = 0;
                fase = 0;
            }
            break;
        }
    }

    //Calculem quants elements hi ha per cada longitud de codi
    std::vector<int> blCount(llargariaMax + 1, 0);
    for (int i = 0; i < vecTriplets.size(); ++i){
        blCount[vecTriplets[i].longitud]++;
    }

    //Calculem la forma numèrica del primer codi per a cada llargaria
    std::vector<int> properCodi(llargariaMax, 0);
    int codi = 0;
    for (int i = 1; i <= llargariaMax; ++i){
        codi = (codi + blCount[i-1]) << 1;
        properCodi[i] = codi;
    }

    //A partir del pas anterior, anem sumant un a un per obtenir el codi de cada paraula
    for (int i = 0; i < vecTriplets.size(); ++i){
        vecTriplets[i].codi = properCodi[vecTriplets[i].longitud]++;
    }

    if (codifica){
        // de texte pla a binari
        for (int i = 0; i < texte.size(); ++i){
            for (int j = 0; j < vecTriplets.size(); ++j){
                
                if (texte[i] == vecTriplets[j].paraula){
                    int mascara = 1 << vecTriplets[j].longitud-1;
                    while (mascara > 0){
                        if (vecTriplets[j].codi & mascara) std::cout << '1';
                        else std::cout << '0';
                        mascara >>= 1;
                    }
                    break;
                }
                
            }
        }
        std::cout << std::endl;
    }
    else{
        // de binari a texte pla
        for (int i = 0, numero = 0, longitud = 0; i < texte.size(); ++i){
            //expandim el codi de binari a numèric
            numero *= 2;
            if (texte[i] == '1') numero++;
            longitud++;

            //Comprovem si hi ha cap paraula que tingui codi de mateix valor i llargària com el que estem analitzant ara mateix
            for (int j = 0; j < vecTriplets.size(); ++j){
                if (numero == vecTriplets[j].codi and longitud == vecTriplets[j].longitud){
                    std::cout << vecTriplets[j].paraula;
                    numero = 0;
                    longitud = 0;
                    break;
                }
            }
        }
        std::cout << std::endl;
    }
}