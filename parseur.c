//#include <stdio.h>
#include <stdlib.h>

struct cellule
{
  int ind;
  int val;
  struct cellule *suiv;
};
typedef struct cellule cell;


#define NB_DOC 70703 

cell tableau[NB_DOC];// = malloc(70703*sizeof(cell)); 

int main() {
  
  for (int i = 0; i < NB_DOC; i++) {
    tableau[i].ind = 0;
    tableau[i].val = 0;
    tableau[i].suiv = NULL;
  }

  return 0;
}
