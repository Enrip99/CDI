# Compressor i descompressor de fitxers per a la pràctica de l'assignatura Compressió de Dades i Imatges de la Facultat d'Informàtica de Barcelona, Universitat Politècnica de Catalunya - BarcelonaTech.

Compressor escrit en C++ per a la pràctica final de CDI.  
Aquest compressor ha estat dissenyat per a i és únicament compatible amb fitxers amb seqüències de nombres, naturals o -1, separats entre ells únicament per un espai o salt de línia. L'ús d'aquest programa amb fitxers de qualsevol altre mena no garanteix un resultat correcte.

## Ús:
* Compressió de fitxer:
    * `./compress.cdi -c entrada.txt sortida.knk`
* Descompressió de fitxer:
    * `./compress.cdi -d entrada.knk sortida.txt`

## Requisits: 
* gcc compatible amb c++20
* make

## Compilació
Executa `make` dins el directori principal per crear l'executable `compress.cdi`.

Classe BinTree feta pel professorat de la Facultat d'Informàtica de Barcelona, disponible a https://pro2.cs.upc.edu/.