#include <iostream>
#include <string>
#include <vector>
#include <regex>

int main(){
    std::string entrada, decodifica;
    std::getline(std::cin, entrada);

    std::vector <std::string> diccionari;
    std::string sortida;

    std::regex exp("\\[(\\d+), *(?:'(.|EOF)'|\"(')\")\\]");
    std::smatch res;

    std::string::const_iterator searchStart( entrada.cbegin() );

    while (regex_search( searchStart, entrada.cend(), res, exp ) ){
        searchStart = res.suffix().first;
        std::string token = res.str(2) + res.str(3), afegir;
        int quantitat = std::stoi(res.str(1));
        if (quantitat){
            afegir += diccionari[quantitat - 1];
        }
        if (token != "EOF"){
            afegir += token;
        }
        decodifica += afegir;
        diccionari.push_back(afegir);
    }
    std::cout << decodifica << std::endl;
}