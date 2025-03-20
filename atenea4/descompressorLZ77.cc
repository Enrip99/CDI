#include <iostream>
#include <string>
#include <vector>

int main(){
    std::string entrada, resultat;
    std::getline(std::cin, entrada);

    int indexEntrada = 0, fase = 0, numLongitud, numDistancia;
    char lletra = ' ';

    while (indexEntrada++ < entrada.length() && lletra != EOF){
        switch (fase){
            case 0: 
                // Trobar lletra
                if (entrada[indexEntrada] == '\''){
                    ++indexEntrada;
                    if (entrada.substr(indexEntrada, 3) == "EOF") {
                        lletra = EOF;
                    }
                    else {
                        lletra = entrada[indexEntrada];
                    }
                    fase = 1;
                }
                break;

            case 1:
                // Trobar número longitud
                if (entrada[indexEntrada] >= '0' && entrada[indexEntrada] <= '9'){
                    numLongitud = entrada[indexEntrada] - '0';

                    fase = 2;
                }
                break;

            case 2:
                // Seguir calculant longitud
                if (entrada[indexEntrada] >= '0' && entrada[indexEntrada] <= '9'){
                    numLongitud *= 10;
                    numLongitud += entrada[indexEntrada] - '0';
                }
                else{
                    fase = 3;
                }
                break;

            case 3:
                // Trobar número distància
                if (entrada[indexEntrada] >= '0' && entrada[indexEntrada] <= '9'){
                    numDistancia = entrada[indexEntrada] - '0';

                    fase = 4;
                }
                break;

            case 4:
                // Seguir calculant distància
                if (entrada[indexEntrada] >= '0' && entrada[indexEntrada] <= '9'){
                    numDistancia *= 10;
                    numDistancia += entrada[indexEntrada] - '0';
                }
                else{
                    fase = 5;
                }
                break;
            
            case 5:
                // Fer tota la pesca
                /*
                if (numDistancia){
                    std::cout << numDistancia << " - " << numLongitud << " - |" << resultat.substr(resultat.length()-numDistancia, numLongitud) << "|" << lletra << std::endl;
                    std::flush(std::cout);
                    resultat.append(resultat.substr(resultat.length()-numDistancia, numLongitud));
                }
                */
                while (numLongitud--){
                    resultat += resultat[resultat.length()-numDistancia];
                }
                //std::cout << lletra << " - " << numLongitud << " - " << numDistancia << std::endl;
                if (lletra != EOF){
                    resultat += lletra;
                }
                //std::cout << resultat << " - " << resultat.length() << std::endl << std::endl;
                fase = 0;
                break;
        }
    }

    std::cout << resultat;

}