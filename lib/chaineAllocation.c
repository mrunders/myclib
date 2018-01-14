#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "chaineAllocation.h"

char* chaineFusion(int nombreDargs, char *chaine, ...){

    va_list va;
    int tailleSortie = 0;
    char *argument = NULL;
    char *sortie = chaineCopie(chaine);

    tailleSortie = strlen(sortie);

    va_start(va, chaine);
    for (; (argument = va_arg(va, char*)) && --nombreDargs ;){
        
        char *c = (char*) realloc(sortie, sizeof(char)* (tailleSortie+=strlen(argument)) );
        if (c == NULL){ fprintf(stderr, "Erreur de realloc dans chaineFusion (abandon)\n"),free(sortie),va_end(va); return NULL;}
        sortie = strcat(c, argument);
    }

    va_end(va);
    return sortie;
}


void chaineAfficheJusqua(char* chaine, char sep){
    for (; *chaine && *chaine != sep; ++chaine) putchar(*chaine);
    putchar('\n');
}

void afficherTableauDeString(char **cc){

    for ( ; *cc ; cc++) printf("[%s]", *cc);
    printf("[NULL]\n");

}

int comparerJusqua(char *c1, char *c2, char finc1){

    for (; *c1 && *c2 && *c1 == *c2 && *c2 != finc1; ++c1, ++c2);
    return *c1 == finc1 && (*c1 == *c2 || !*c2);
}