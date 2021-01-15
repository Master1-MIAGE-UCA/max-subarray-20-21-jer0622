HIRTH Jeremy
N°étu : 21708412
Master 1 Informatique

struct tablo *allocateTablo(int size) : Non parallèle
    Fonction qui alloue un tableau


void duplicateArray(struct tablo *src, struct tablo *dest) : Non parallèle
    Fonction qui permet de dupliquer un tableau


int max(int a, int b) : Non parallèle
    Fonction qui renvoie le max entre deux nombres


void montee(struct tablo *source, struct tablo *destination, int mode) : Parallèle
    Phase montée


void descente(struct tablo *a, struct tablo *b, int mode) : Parallèle
    Phase descente


void final(struct tablo *a, struct tablo *b, int mode) : Parallèle
    Phase Final


void reverseArray(struct tablo *array) : Non parallèle
    Fonction qui inverse les éléments d'un tableau


void prefix(struct tablo *src, struct tablo *dest, int mode) : Non parallèle
    Fonction prefix en fonction du mode
    1 ===> sum() 
    2 ===> max()
    Pour calculer le suffixe on reverse le tableau avant.


void subSequence(struct tablo *tableau) : Parallèle
    Deux sections parallèle :
    ===> Les tableaux "psum" et "smax" peuvent se calculer indépendamment des tableaux "ssum" et "pmax"