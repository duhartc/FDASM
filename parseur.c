#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

#define NB_DOC 70703 
#define TAILLE_VOC 10000 //TODO: trouver réellement

doc tableau[NB_DOC]; // = malloc(70703*sizeof(cell)); 

void affiche_tableau(doc * tab) {
    for (int i = 0; i < NB_DOC; i++) {
        printf("%d ", tab[i].classe);
        cell* cour = tab[i].vect;
        while (cour != NULL) {
            printf("%d:%d ", cour->ind, cour->val);
            cour = cour->suiv;
        }
        printf("\n");
    }
}

int main() {
    FILE* base = NULL;
    base = fopen("BaseReuters-29", "r");
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
                //printf("%s", indVal);
            }
            prec->suiv = NULL;
            tableau[i] = *newDoc;
            cour = NULL;
            i++;
        }
        fclose(base);
        //affiche_tableau(tableau);
    } else {
        printf("Erreur lors de l'ouverture du fichier");
    }

    return 0;
}
