#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

struct cellule {
    int ind;
    int val;
    struct cellule *suiv;
};

typedef struct cellule cell;

struct doc {
    int classe;
    struct cellule *vect;
};
typedef struct doc doc;

struct divided_collection {
    doc *entrainement;
    doc *test;
};
typedef struct divided_collection divided_collection;

#define NB_DOC 70703 
#define TAILLE_DOC_MAX 10000 // arbitraire
#define TAILLE_ENTRAINEMENT 52500
#define TAILLE_TEST 18203
#define NB_CLASS 29

doc tableau[NB_DOC]; 

void affiche_tableau(doc * tab, int taille) {
    for (int i = 0; i < taille; i++) {
        printf("%d ", tab[i].classe);
        cell* cour = tab[i].vect;
        while (cour != NULL) {
            printf("%d:%d ", cour->ind, cour->val);
            cour = cour->suiv;
        }
        printf("\n");
    }
}
void melanger(int* tableau, int taille){
    int nombre_tire=0;
    int temp=0;
    for(int i = 0; i< taille;i++){
        nombre_tire=rand()%taille;
        // On échange les contenus des cases i et nombre_tire
        temp = tableau[i];
        tableau[i] = tableau[nombre_tire];
        tableau[nombre_tire]=temp;
    }

}
divided_collection divide_tableau(doc * tab) {
    divided_collection col;
    srand(time(NULL));
    col.entrainement = malloc(TAILLE_ENTRAINEMENT * sizeof(doc));
    col.test = malloc(TAILLE_TEST * sizeof(doc));
    int *alea = malloc(NB_DOC*sizeof(int));
    int i = 0;
    for(i = 0; i< NB_DOC; i++){
        alea[i]=i;
    }
    melanger(alea, NB_DOC);
    for (int i = 0; i < TAILLE_TEST; i++){
        col.test[i] = tab[alea[i]];
    }
    for (int i = TAILLE_TEST; i < NB_DOC; i++) {
        col.entrainement[i-TAILLE_TEST] = tab[alea[i]];
    }
    free(alea);
    return col;
}

int parse_file(char *name, int * doc_per_class) {
    FILE* base = NULL;
    base = fopen(name, "r");
    int i = 0;
    int max = 0;
    if (base != NULL) {
        char *line = malloc(sizeof (char) *TAILLE_DOC_MAX);
        char *val;
        char *indVal;
        cell* cour;
        cell* prec;
        char *indVal_end, *val_end;
        while (fgets(line, TAILLE_DOC_MAX, base) != NULL) {
            // on lit ligne par ligne
            // une ligne correspond à un document
            indVal = strtok_r(line, " ", &indVal_end);
            tableau[i].classe = atoi(indVal);
            doc_per_class[tableau[i].classe - 1] += 1;
            cell* firstCell = malloc(sizeof (cell));
            tableau[i].vect = firstCell;
            cour = firstCell;
            indVal = strtok_r(NULL, " ", &indVal_end);
            while (strlen(indVal) >= 3) {
                // on lit mot par mot
                char *indValCpy = malloc(sizeof (char) *strlen(indVal) + 1);
                strcpy(indValCpy, indVal);
                val = strtok_r(indValCpy, ":", &val_end);
                cour->ind = atoi(val);
                // on sotcke le maximum pour obtenir la dimension
                if (atoi(val) > max) max = atoi(val);
                val = strtok_r(NULL, ":", &val_end);
                cour->val = atoi(val);
                cell* newCell = malloc(sizeof (cell));
                cour->suiv = newCell;
                prec = cour;
                cour = newCell;
                indVal = strtok_r(NULL, " ", &indVal_end);
                free(indValCpy);
            }
            prec->suiv = NULL;
            free(cour);
            cour = NULL;
            i++;
        }
        free(line);
        fclose(base);
    } else {
        printf("Erreur lors de l'ouverture du fichier");
    }
    return max;
}

void free_collection(doc * tab, int taille) {
    for(int i = 0; i< taille;i++){
       cell* cour = tab[i].vect;
       cell* courNext;
        while (cour != NULL) {
            courNext = cour->suiv;
            free(cour);
            cour = courNext;
        }
    }
}


int main() {
    int doc_per_class[NB_CLASS] = {0};
    int dimension; //taille du vocabulaire (considèrée ici comme le max des indices)
    // QUESTION : Est ce vraiment la bonne considèration ? 
    dimension = parse_file("BaseReuters-29", doc_per_class); 
    
    //printf("Dimension du problème: %d", dimension);
    //Ci-dessous affichage et vérification du nombre de documents par classe
    /*int sum = 0;
    for (int i = 0; i < NB_CLASS; i++) {
        printf("%d : %d\n", i, doc_per_class[i]);
        sum += doc_per_class[i];
    }
    printf("Somme : %d", sum);*/
    
    //affiche_tableau(tableau, NB_DOC);
    /* le tableau des documents est en variable globale 
     -> eventuellement à changer */
    divided_collection col = divide_tableau(tableau);
    //affiche_tableau(col.test, TAILLE_TEST);
    //affiche_tableau(col.entrainement, TAILLE_ENTRAINEMENT);
    free_collection(tableau, NB_DOC);
    free(col.test);
    free(col.entrainement);
    return 0;
}
