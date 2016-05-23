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
	fflush(stdout);
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
	    tableau[i].taille_doc =0;
            cour = firstCell;
            indVal = strtok_r(NULL, " ", &indVal_end);
            while (strlen(indVal) >= 3) {
                // on lit mot par mot
                char *indValCpy = malloc(sizeof (char) *strlen(indVal) + 1);
                strcpy(indValCpy, indVal);
                val = strtok_r(indValCpy, ":", &val_end);
                cour->ind = atoi(val);
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
	    // printf("taille du document %i : %i \n", i,tableau[i].taille_doc); 
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



void EstimationMultinomiale(doc *train, double **theta, double *pi, int *n) {
  int k=0; //indice de la classe
  cell *tmp; //représente une composante du vecteur ind-val, variable tampon
  int ind_tmp=1;

  for (int i=0; i<TAILLE_ENTRAINEMENT; i++) {
    k = train[i].classe; //on récupère la classe du document i
    pi[k-1] += 1;
    tmp = train[i].vect;
    for (int j=1; j <= train[i].taille_doc; j++) {
      //parcours de la liste chainée pour aller au j-ème élément
      while (ind_tmp < j) {
	tmp = tmp->suiv;
	ind_tmp++;
      }
      //calcul effectif
      theta[tmp->ind-1][k-1] += (double) tmp->val;
    }
    n[k-1] += theta[tmp->ind-1][k-1];
    ind_tmp = 1;
  }

  //normalisation
  for (int k=0; k<NB_CLASS; k++) {
    pi[k] /= TAILLE_ENTRAINEMENT; //probabilité de la classe k !
  }
  for (int i=0; i<TAILLE_ENTRAINEMENT; i++) {
    k = train[i].classe;
    tmp = train[i].vect;
    for (int j=1; j <= train[i].taille_doc; j++) {
      //parcours de la liste chainée pour aller au j-ème élément
      while (ind_tmp < j) {
	tmp = tmp->suiv;
	ind_tmp++;
      }
      theta[tmp->ind-1][k-1] /= n[k-1];
    }
    ind_tmp = 1;
  }
}

void PredictionMultinomiale(doc *test, double **theta, double *pi, double *pbc) {
  int classe=0; //classe prédite
  double max=-pow(10,20);
  double v[NB_CLASS]={0}; //vecteur "tampon" servant à calculer le max
  cell *tmp;
  int ind_tmp=1;
  for (int i=0; i<TAILLE_TEST; i++) {
    for (int k=1; k<=NB_CLASS; k++) {
      v[k-1]=log(pi[k-1]);
      tmp = test[i].vect;
      for (int j=1; j <= test[i].taille_doc; j++) {
	//parcours de la liste chainée pour aller au j-ème élément
	while (ind_tmp < j) {
	  tmp = tmp->suiv;
	  ind_tmp++;
	}
	v[k-1] += (tmp->val)*log(theta[tmp->ind-1][k-1]);
	//calcul du max et de la classe prédite
	if (v[k-1] > max) {
	  max = v[k-1];
	  classe = k;
	}
      }
      ind_tmp=1;

      //On comptabilise les classes correctement prédites
      if (classe == test[i].classe) {
	pbc[classe-1] += 1.0;
      }
    }
  }
}

/*
void EstimationBernoulli(doc *train, double **theta, double *pi) {
  int k=0; //indice de la classe
  cell *tmp;
  int ind_tmp=1;

  for (int i=0; i<TAILLE_ENTRAINEMENT; i++) {
    k = train[i].classe;
    pi[k-1] += 1;
    tmp = train[i].vect;
    for (int j=1; j <= train[i].taille_doc; j++) {
      //parcours de la liste chainée pour aller au j-ème élément
      while (ind_tmp < j) {
	tmp = tmp->suiv;
	ind_tmp++;
      }
      theta[tmp->ind-1][k-1] += 1;
    }
    ind_tmp = 1;
  }

  //normalisation
  for (int k=0; k<NB_CLASS; k++) {
    pi[k] /= TAILLE_ENTRAINEMENT ;
  }
  for (int i=0; i<TAILLE_ENTRAINEMENT; i++) {
    k = train[i].classe;
    tmp = train[i].vect;
    for (int j=1; j <= train[i].taille_doc; j++) {
      //parcours de la liste chainée pour aller au j-ème élément
      while (ind_tmp < j) {
	tmp = tmp->suiv;
	ind_tmp++;
      }
      theta[tmp->ind-1][k-1] /= n[k-1];
    }
    ind_tmp = 1;
  }
}

void PredictionBernoulli(doc *test, double **theta, double *pi, double *pbc) {
  int classe=0;
  double max=-pow(10,20);
  double v[NB_CLASS]={0};
  cell *tmp;
  int ind_tmp=1;
  for (int i=0; i<TAILLE_TEST; i++) {
    for (int k=1; k<=NB_CLASS; k++) {
      v[k-1]=log(pi[k-1]);
      tmp = test[i].vect;
      for (int j=1; j <= test[i].taille_doc; j++) {
      //parcours de la liste chainée pour aller au j-ème élément
      while (ind_tmp < j) {
	tmp = tmp->suiv;
	ind_tmp++;
      }
      v[k-1] += log(theta[tmp->ind-1][k-1]);
      
      if (v[k-1] > max) {
	max = v[k-1];
	classe = k;
      }
      }
      ind_tmp=1;
    
    if (classe == test[i].classe) {
      pbc[classe-1] += 1.0;
    }
  }
  }
}
*/

int main() {
    int doc_per_class[NB_CLASS] = {0};
    int dimension; //taille du vocabulaire (V)
   
    //déclaration de variables utiles au calcul multinomiale
    double **Theta= malloc(141144*sizeof(double*));
    double *Pi = malloc(NB_CLASS*sizeof(double));
    int* N = malloc(NB_CLASS*sizeof(int));
    double *PBC = malloc(NB_CLASS*sizeof(double));
    //initialisation des variables
    for (int i=0; i<141144; i++) {
      Theta[i] = malloc(NB_CLASS*sizeof(double));
    }
    for (int i=0; i<141144; i++) {
      for (int j=0; j<NB_CLASS; j++) {
	Theta[i][j] = 0;
      }
    }
    for (int j=0; j<NB_CLASS; j++) {
	Pi[j] = 0;
	N[j] = 0;
	PBC[j] = 0;
    }
   
    dimension = parse_file("BaseReuters-29", doc_per_class); 
    //printf("Dimension du problème: %d", dimension);
    //pour afficher V, on trouve 141144

    //On crée deux collections : train et test
    divided_collection col;
    //Vecteur stockant les performances pour les 20 itérations
    double *perf = malloc(20*sizeof(double));
    double moyenne_perf =0;

    for (int m=0; m<20; m++) {
      //on scinde les données en deux
      col = divide_tableau(tableau);
      //estimation : on remplit Pi, Theta, N
      EstimationMultinomiale(col.entrainement,Theta, Pi, N);
      //prediction : on remplit PBC à partir de Theta et Pi
      PredictionMultinomiale(col.test, Theta, Pi, PBC);
    
      //calcul de performance
      double sumBC=0;
      for (int l=0; l<NB_CLASS;l++) {
	sumBC += PBC[l];
      }
      perf[m] = sumBC/TAILLE_TEST;
      printf("Taux de bonne classification %i: %f \n", m, perf[m]);
      fflush(stdout);
      //on reinitialise PBC à chaque itération
      for (int j=0; j<NB_CLASS; j++) {
	PBC[j] = 0;
      }
      moyenne_perf += perf[m]/20;
    }
    printf("Taux de bonne classification moyen : %f \n", moyenne_perf);


    //désallocation mémoire
    free_collection(tableau, NB_DOC);
    free(col.test);
    free(col.entrainement);
    for (int i=0; i<141144; i++) {
      free(Theta[i]);
    }
    free(Theta);
    free(Pi);
    free(N);
    free(PBC);
    free(perf);
    return 0;
}
