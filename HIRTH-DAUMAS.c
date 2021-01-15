#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// HIRTH Jeremy
// N°étu : 21708412
// Master 1 Informatique

struct tablo {
    int *tab;
    int size;
};


// Fonction qui alloue un tableau
struct tablo *allocateTablo(int size) {
    struct tablo *tmp = malloc(sizeof(struct tablo));
    tmp->size = size;
    tmp->tab = malloc(size * sizeof(int));
    tmp->tab[0]=0;
    return tmp;
}

// Fonction qui permet de dupliquer un tableau
void duplicateArray(struct tablo *src, struct tablo *dest) {
    for (int i = 0; i < src->size; i++) {
        dest->tab[i] = src->tab[i];
    }
}

// Fonction qui renvoie le max entre deux nombres
int max(int a, int b) {
    if (a > b) return a;
    return b;
}

// Phase montée
void montee(struct tablo *source, struct tablo *destination, int mode) {
    // Remplissage du tableau destination de taille 2 * m
    for (int i = 0; i < source->size; i++) {
        destination->tab[i + source->size] = source->tab[i];
    }

    // Boucle de calcul pour la montee dans l'arbre/tableau
    for (int l = (int)log2(source->size) - 1; l >= 0; l--) {
        // Borne inférieur et supérieur
        int inf = pow(2, l);
        int sup = pow(2, l+1) - 1;

        // On fait le test avant pour éviter de le refaire à chaque fois dans la bouvle for
        if (mode == 1) {
            #pragma omp parallel for
            for (int j = inf; j <= sup; j++)
                destination->tab[j] = destination->tab[2*j] + destination->tab[2*j + 1];
        }
        else {
            #pragma omp parallel for
            for (int j = inf; j <= sup; j++)
                destination->tab[j] = max(destination->tab[2*j], destination->tab[2*j + 1]);
        }
    }
}

// Phase descente
void descente(struct tablo *a, struct tablo *b, int mode) {
    // Fixe la valeur neutre en fonction du mode
    if (mode == 1) 
        b->tab[1] = 0;               // Valeur neutre pour la somme
    else 
        b->tab[1] = INT_MIN;         // Valeur neutre pour le max (le plus petit int)

    // Boucle de calcul pour la déscente
    for (int l = 1; l <= (int)log2(a->size/2); l++) {
        int inf = pow(2, l);
        int sup = pow(2, l+1) - 1;

        // On fait le test avant pour éviter de le refaire à chaque fois dans la bouvle for
        if (mode == 1) {
            #pragma omp parallel for
            for (int j =inf; j <=sup ; j++)
                if (j % 2 == 0)
                    b->tab[j] = b->tab[j/2];
                else
                    b->tab[j] = b->tab[(j-1)/2] + a->tab[j-1];
        }
        else {
            #pragma omp parallel for
            for (int j =inf; j <=sup ; j++)
                if (j % 2 == 0)
                    b->tab[j] = b->tab[j/2];
                else
                    b->tab[j] = max(b->tab[(j-1)/2], a->tab[j-1]);
        }
    }
}

// Phase Final
void final(struct tablo *a, struct tablo *b, int mode) {
    int m = (int)log2(a->size/2);

    // Borne inférieur et supérieur
    int inf = pow(2, m);
    int sup = pow(2, m+1) - 1;

    // On fait le test avant pour éviter de le refaire à chaque fois dans la bouvle for
    if (mode == 1) {
        #pragma omp parallel for
        for (int j = inf; j <= sup; j++)
            b->tab[j] = b->tab[j] + a->tab[j];
    }
    else {
        #pragma omp parallel for
        for (int j = inf; j <= sup; j++)
            b->tab[j] = max(b->tab[j], a->tab[j]);
    }
}


// Fonction qui inverse les éléments d'un tableau
void reverseArray(struct tablo *array) {
	int i, j, temp;
	for (i = 0, j = array->size - 1; i < j; i++,j--) {
		temp = array->tab[i];
		array->tab[i] = array->tab[j];
		array->tab[j] = temp;
	}
}


// Fonction prefix en fonction du mode
// 1 ===> sum() 
// 2 ===> max()
void prefix(struct tablo *src, struct tablo *dest, int mode) {
    // Phase montée
    struct tablo *a = allocateTablo(src->size * 2);
    montee(src, a, mode);

    // Phase déscente
    struct tablo *b = allocateTablo(src->size * 2);
    descente(a, b, mode);

    // Phase Final
    final(a, b, mode);

    // Récupération de la seconde moitié du tableau
    for (int i = 0; i < src->size; i ++) {
        dest->tab[i] = b->tab[i + src->size]; 
    }
}

// Calcul de la sous sequence max
void subSequence(struct tablo *tableau) {
    // Allocation des tableaux utilisé
    struct tablo *psum = allocateTablo(tableau->size);
    struct tablo *smax = allocateTablo(tableau->size);
    struct tablo *ssum = allocateTablo(tableau->size);
    struct tablo *pmax = allocateTablo(tableau->size);

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            // ----------------------- Calcul de PSUM -----------------------
            prefix(tableau, psum, 1);


            // ----------------------- Calcul de SMAX -----------------------
            struct tablo *psumReverse = allocateTablo(tableau->size);
            duplicateArray(psum, psumReverse);
            reverseArray(psumReverse);
            prefix(psumReverse, smax, 2);
            reverseArray(smax);
            free(psumReverse);
        }

        #pragma omp section
        {
            // ----------------------- Calcul de SSUM -----------------------
            struct tablo *tabReverse = allocateTablo(tableau->size);
            duplicateArray(tableau, tabReverse); 
            reverseArray(tabReverse);
            prefix(tabReverse, ssum, 1);
            reverseArray(ssum);
            free(tabReverse);


            // ----------------------- Calcul de PMAX -----------------------
            prefix(ssum, pmax, 2);
        }
    }

    
    // ----------------------- Calcul de M -----------------------
    struct tablo *m = allocateTablo(tableau->size);
    #pragma omp parallel for
    for (int i = 0; i < tableau->size; i++) {
        m->tab[i] = (pmax->tab[i] - ssum->tab[i] + tableau->tab[i]) +
                    (smax->tab[i] - psum->tab[i] + tableau->tab[i]) - tableau->tab[i];
    }
    

    // ----------------------- Recuperation du max et de son indice de debut -----------------------
    int debut = 0;
    int max = m->tab[0];
    for (int i = 1; i < m->size; i++) {
        if (m->tab[i] > max) {
            max = m->tab[i];
            debut = i;
        }
    }

    // ----------------------- Affichage du sous-tableau -----------------------
    printf("%d ", max);
    for (int i = debut; i < m->size && m->tab[i] == max; i++) {
        printf("%d ", tableau->tab[i]);
    }
    printf("\n");

    // Libération de la mémoire
    free(psum);
    free(ssum);
    free(smax);
    free(pmax);
    free(m);
}


// Argument 1 ====> Fichier contenant le tableau
int main(int argc, char **argv) {
    // Check des argument
    if (argc != 2) {
        printf("Usage %s [PATH ARRAY]", argv[0]);
        return 1;
    }

    // Ouverture du fichier en lecture
    FILE *fichier = fopen(argv[1], "r");

    // On parcours une première fois le fichier pour connaitre la taille du tableau
    int a;
    int taille = 0;
    if (fichier != NULL) 
        while (fscanf(fichier, "%d", &a) == 1) {
            taille++;
        }

    // On alloue un tableau de bonne taille
    struct tablo *tableau = allocateTablo(taille);

    // On se replace au début du fichier
    fseek(fichier, 0, SEEK_SET);

    // On parcours une deuxième fois pour prélever le tableau
    int i = 0;
    if (fichier != NULL)
        while (fscanf(fichier, "%d", &a) == 1) {
            tableau->tab[i] = a;
            i++;
        }
    fclose(fichier);

    // Calcul de la sous sequence max et affichage
    subSequence(tableau);

    return 1;
}
