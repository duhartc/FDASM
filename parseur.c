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
#define TAILLE_VOC 10000 //TODO: trouver réellement
#define TAILLE_ENTRAINEMENT 52500
#define TAILLE_TEST 18203

doc tableau[NB_DOC]; // = malloc(70703*sizeof(cell)); 

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
    return col;
}

void parse_file(char *name) {
    FILE* base = NULL;
    base = fopen(name, "r");
    int i = 0;
    if (base != NULL) {
        char *line = malloc(sizeof (char) *TAILLE_VOC);
        char *val;
        char *indVal;
        cell* cour;
        cell* prec;
        char *indVal_end, *val_end;
        doc* newDoc = malloc(sizeof (doc));
        while (fgets(line, TAILLE_VOC, base) != NULL) {
            // on lit ligne par ligne
            // une ligne correspond à un document
            indVal = strtok_r(line, " ", &indVal_end);
            newDoc->classe = atoi(indVal);
            cell* firstCell = malloc(sizeof (cell));
            newDoc->vect = firstCell;
            cour = firstCell;
            indVal = strtok_r(NULL, " ", &indVal_end);
            while (strlen(indVal) >= 3) {
                // on lit mot par mot
                char *indValCpy = malloc(sizeof (char) *strlen(indVal) + 1);
                strcpy(indValCpy, indVal);
                val = strtok_r(indValCpy, ":", &val_end);
                cour->ind = atoi(val);
                val = strtok_r(NULL, ":", &val_end);
                cour->val = atoi(val);
                cell* newCell = malloc(sizeof (cell));
                cour->suiv = newCell;
                prec = cour;
                cour = newCell;
                indVal = strtok_r(NULL, " ", &indVal_end);
            }
            prec->suiv = NULL;
            tableau[i] = *newDoc;
            cour = NULL;
            i++;
        }
        fclose(base);
    } else {
        printf("Erreur lors de l'ouverture du fichier");
    }
}

int main() {
    parse_file("BaseReuters-29");
    affiche_tableau(tableau, NB_DOC);
    /* le tableau des documents est en variable globale 
     -> eventuellement à changer */
    divided_collection col = divide_tableau(tableau);
    //affiche_tableau(col.test, TAILLE_TEST);
    //affiche_tableau(col.entrainement, TAILLE_ENTRAINEMENT);
    return 0;
}
