#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <stdio.h>
#include "LectorBits.hpp"
#include "DimensionsIO.hpp"
#include "BinTree.hpp"

#define estructuraParella std::pair<__uint8_t, unsigned long long int>
#define estructuraArbre BinTree < estructuraParella >
#define estructuraCache std::map<__uint8_t, std::vector<bool> >


void errorLectura(){
    perror("Error en llegir el fitxer.");
    exit(1);
}


estructuraArbre getLiderBloc(estructuraArbre root, estructuraArbre objectiu){
    std::queue <estructuraArbre> cua;
    cua.push(root);

    while(!cua.empty()){
        if(cua.front().equals(objectiu)) {
            return estructuraArbre(estructuraParella{0,0});
        }
        else if (cua.front().value().second == objectiu.value().second && 
                cua.front().right().empty() == objectiu.right().empty()) {
            return cua.front();
        }
        else if (! cua.front().right().empty()){
            cua.push(cua.front().right());
            cua.push(cua.front().left());
            cua.pop();
        }
        else {
            cua.pop();
        }
    }
    return estructuraArbre(estructuraParella{0,0});
}


estructuraArbre getLiderFullaSuperior(estructuraArbre root, estructuraArbre objectiu){
    std::queue <estructuraArbre> cua;
    cua.push(root);

    while(!cua.empty()){
        if(cua.front().equals(objectiu)) {
            return estructuraArbre(estructuraParella{0,0});
        }
        else if (cua.front().value().second == objectiu.value().second + 1 && 
                cua.front().right().empty()) {
            return cua.front();
        }
        else if (! cua.front().right().empty()){
            cua.push(cua.front().right());
            cua.push(cua.front().left());
            cua.pop();
        }
        else {
            cua.pop();
        }
    }
    return estructuraArbre(estructuraParella{0,0});
}


estructuraArbre getLiderInteriorIgual(estructuraArbre root, estructuraArbre objectiu){
    std::queue <estructuraArbre> cua;
    cua.push(root);

    while(!cua.empty()){
        if(cua.front().equals(objectiu)) {
            return estructuraArbre(estructuraParella{0,0});
        }
        else if (cua.front().value().second == objectiu.value().second && 
                ! cua.front().right().empty()) {
            return cua.front();
        }
        else if (! cua.front().right().empty()){
            cua.push(cua.front().right());
            cua.push(cua.front().left());
            cua.pop();
        }
        else {
            cua.pop();
        }
    }
    return estructuraArbre(estructuraParella{0,0});
}


void LliscaIncrementa(estructuraArbre root, estructuraArbre & node){
    estructuraArbre pareOrig = node.parent();

    if (node.right().empty()){
        //És fulla; lliscar fins el més alt dels interns de mateixa mida

        estructuraArbre lider = getLiderInteriorIgual(root, node);
        if (lider.value().second != 0) {
            lider.swap(node);
            node = lider;
        }

        estructuraParella temp = node.value();
        ++temp.second;
        node.setValue(temp);

        node = node.parent();
    }
    else{
        //És intern, lliscar fins la fulla més alta de mida n+1
        
        estructuraArbre lider = getLiderFullaSuperior(root, node);
        if (lider.value().second != 0) {
            lider.swap(node);
            node = lider;
        }

        estructuraParella temp = node.value();
        ++temp.second;
        node.setValue(temp);

        node = pareOrig;

    }
}


__uint8_t llegeigByte(LectorBits & lector){
    __uint8_t properByte = 0x00;
    signed char llegit;
    for (int i = 7; i >= 0; --i){
        llegit = lector.llegeixBit();
        if (llegit == -1) errorLectura();
        properByte |= (llegit << i);
    }
    return properByte;
}


int main(int argc, char *argv[]){
    
    if (argc != 3){
        std::cout << "Us: " << argv[0] << " <input> <output>" << std::endl;
        exit(1);
    }


    FILE * sortida = fopen(argv[2], "w");
    __uint8_t bufferEscriptura[midaBuffer] = {0};
    int bytesAEscriure;
    //Obrim fitxer per escriure, segon argument de la linia de comanda.
    //Variables per assistir a l'escriptura a baix nivell.

    LectorBits lector(argv[1]);
    signed char llegit;
    //Objecte per llegir el fixter comprimit a disc, primer argument de la linia de comanda.

    estructuraArbre arbreAdaptatiu (estructuraParella {0,1});
    //Arbre sobre el qual crearem la codificació de Huffman adaptativa.
    //first: byte que codifica en cas de ser una fulla no NYT.
    //second: nombre d'iteracions del node. 0 per NYT.
    //S'inicia com a mida 1, i tan bon punt comencem, li afegim NYT i primer byte.

    estructuraArbre arbreRecorrer = estructuraArbre(arbreAdaptatiu);
    //Arbre que anirem movent mentre es recorre l'arbre.


    //Procès principal
    //
    //Llegim el fitxer d'entrada sencer
    //Abans, primer llegim el primer byte

    arbreAdaptatiu.addChildren(
        estructuraArbre (estructuraParella {0,0}),
        estructuraArbre (estructuraParella {llegeigByte(lector),1})
    );

    bytesAEscriure = 0;
    bufferEscriptura[bytesAEscriure++] = arbreAdaptatiu.right().value().first;
    estructuraArbre fullaIncrementar;

    while(llegit = lector.llegeixBit(), llegit != -1){
        if (llegit == 0){
            arbreRecorrer = arbreRecorrer.left();
        }
        else{
            arbreRecorrer = arbreRecorrer.right();
        }

        if (arbreRecorrer.left().empty()){

            fullaIncrementar = estructuraArbre();

            if (arbreRecorrer.value().second == 0){
                // NYT
                __uint8_t noubyte = llegeigByte(lector);
                bufferEscriptura[bytesAEscriure++] = noubyte;
                if (bytesAEscriure == midaBuffer){
                    fwrite(bufferEscriptura, sizeof(__uint8_t), bytesAEscriure, sortida);
                    bytesAEscriure = 0;
                    for (int i = 0; i < midaBuffer; ++i) bufferEscriptura[i] = 0;
                }

                arbreRecorrer.addChildren(
                    estructuraArbre (estructuraParella {0,0}),
                    estructuraArbre (estructuraParella {noubyte,0})
                );
                fullaIncrementar = arbreRecorrer.right();
            }
            else{
                // Fulla real
                bufferEscriptura[bytesAEscriure++] = arbreRecorrer.value().first;
                if (bytesAEscriure == midaBuffer){
                    fwrite(bufferEscriptura, sizeof(__uint8_t), bytesAEscriure, sortida);
                    bytesAEscriure = 0;
                    for (int i = 0; i < midaBuffer; ++i) bufferEscriptura[i] = 0;
                }

                // Swap líder bloc

                estructuraArbre lider = getLiderBloc(arbreAdaptatiu, arbreRecorrer);
                if (lider.value().second != 0){
                    lider.swap(arbreRecorrer);
                    arbreRecorrer = lider;
                }

                if (arbreRecorrer.parent().left().value().second == 0){
                    fullaIncrementar = arbreRecorrer;
                    arbreRecorrer = arbreRecorrer.parent();
                }

            }

            while (! arbreRecorrer.empty()){
                LliscaIncrementa(arbreAdaptatiu, arbreRecorrer);
            }

            if (! fullaIncrementar.empty()){
                LliscaIncrementa(arbreAdaptatiu, fullaIncrementar);
            }

            arbreRecorrer = estructuraArbre(arbreAdaptatiu);
        }
    }

    if (bytesAEscriure) std::fwrite(bufferEscriptura, sizeof(__uint8_t), bytesAEscriure, sortida);
    std::fclose(sortida);

    if (! arbreAdaptatiu.equals(arbreRecorrer)){
        perror("S'ha arribat al final del fitxer sense descomprimir totes les dades.\n \
        El fitxer comprimit es podria haver corromput.");
        exit(1);
    }

    return 0;

}