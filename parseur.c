#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct cellule {
    int ind;
    int val;
    struct cellule *suiv;
};

typedef struct cellule cell;

struct doc {
    int classe;
    int taille_doc;
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

//Scinde en deux collections (train et test) 
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
	    tableau[i].taille_doc =0;
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
		//taille du document
		tableau[i].taille_doc++;
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


void EstimationMultinomiale(doc *train, double theta[][NB_CLASS], double *pi, int *n) {
  int k=0; //indice de la classe
  cell *tmp;
  int ind_tmp=1;

    printf("AAAAAAAAAAAAAAA");
  for (int i=0; i<TAILLE_ENTRAINEMENT; i++) {
    k = train[i].classe;
    pi[k] += 1.0;
    tmp = train[i].vect;
    for (int j=1; j <= train->taille_doc; j++) {
      //parcours de la liste chainée pour aller au j-ème élément
      while (ind_tmp < j) {
	tmp = tmp->suiv;
	ind_tmp++;
      }
      theta[tmp->ind][k] += (double) tmp->val;
      n[k] +=  round(theta[tmp->ind][k]);
    }
  }

  //normalisation
  for (int i=0; i<TAILLE_ENTRAINEMENT; i++) {
    k = train[i].classe;
    pi[k] = pi[k]/TAILLE_ENTRAINEMENT;
    tmp = train[i].vect;
    for (int j=1; j <= train->taille_doc; j++) {
      //parcours de la liste chainée pour aller au j-ème élément
      while (ind_tmp < j) {
	tmp = tmp->suiv;
	ind_tmp++;
      }
      theta[tmp->ind][k] /= (double) n[k];
    }
  }
}

void PredictionMultinomiale(doc *test, double theta[][NB_CLASS], double *pi, double *pbc) {
  int classe=0;
  double v[NB_CLASS] = {0};
  double max=-pow(10,20);
  cell *tmp;
  int ind_tmp=1;
  for (int i=0; i<TAILLE_TEST; i++) {
    for (int k=1; k<=NB_CLASS; k++) {
      v[k]=log(pi[k]);
      for (int j=1; j <= test->taille_doc; j++) {
      //parcours de la liste chainée pour aller au j-ème élément
      while (ind_tmp < j) {
	tmp = tmp->suiv;
	ind_tmp++;
      }
      v[k] += ((double)tmp->val)*log(theta[tmp->ind][k]);
      if (v[k] > max) {
	max = v[k];
	classe = k;
      }
      }
    }
    if (classe == test[i].classe) {
      pbc[classe] += 1;
    }
  }
}

int main() {
    int doc_per_class[NB_CLASS] = {0};
    //taille du vocabulaire : V = 141 144
    //printf("Dimension du problème: %d \n", dimension);
    int dimension = parse_file("BaseReuters-29", doc_per_class);
    //taille du problème : CardTheta =dimension*NB_CLASS = 4 093 176;
    //initialisation des données
    double Theta[141144][NB_CLASS] = {{1.0}}; 
    double Pi[NB_CLASS] = {0};
    int N[NB_CLASS] = {0};
    double PBC[NB_CLASS] = {0};

    //printf("Dimension du problème: %d \n", dimension);
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

    EstimationMultinomiale(col.entrainement,Theta, Pi, N);
    PredictionMultinomiale(col.test, Theta, Pi, PBC);
    //calcul de performance
    double sumBC=0;
    for (int l=0; l<NB_CLASS;l++) {
      sumBC += PBC[l];
    }
    double perf = sumBC/TAILLE_TEST;
    printf("Taux de bonne classification : %f \n", perf);
      
    //désallocation mémoire
    free_collection(tableau, NB_DOC);
    free(col.test);
    free(col.entrainement);
    return 0;
}
