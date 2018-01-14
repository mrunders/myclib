#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "fichierDeConfiguration.h"

char DELIMITEUR_CLEF_VALEUR = ':';

int fdc_ouvrir(char *chemin){

    int fd = open(chemin, O_RDONLY);

    if (fd == -1){ 
        fprintf(stderr, "Le fichier [%s] n'a pu etre ouvert (abandon)\n", chemin);
    }

    return fd;
}

int fdc_fermer(int fd){

    return close(fd);
}

void fdc_changerDelimiteur(char nouv_del){

    DELIMITEUR_CLEF_VALEUR = nouv_del;
}

int fdc_delimiteur(){

    return DELIMITEUR_CLEF_VALEUR;
}

int __lireJusuaDelimiteur(int fd, char **chaine, char delimiteur){

    int a = 0;
    int nbcaracpchaine = 0;
    int limitepchar = NOMBRE_DE_CARACTERE_EN_EXTENTION;
    char caracCourant = '\0';
    char *pchaine = NULL;

    pchaine = (char*) calloc(NOMBRE_DE_CARACTERE_EN_EXTENTION, sizeof(char));

    for (; (read(fd, &caracCourant, 1)) && caracCourant != delimiteur ; ){

        a = 1;
        if (nbcaracpchaine == limitepchar -2){

            char *p = (char*) realloc(pchaine, limitepchar + NOMBRE_DE_CARACTERE_EN_EXTENTION);
            if (p == NULL){fprintf(stderr, "pd de realloc\n");exit(127);}
            memset(p+limitepchar, 0, NOMBRE_DE_CARACTERE_EN_EXTENTION);
            limitepchar += NOMBRE_DE_CARACTERE_EN_EXTENTION;
            pchaine = p;
        }

        pchaine[nbcaracpchaine++] = caracCourant;
    }

    *chaine = pchaine;

    if (!a) free(pchaine);
    return a;
}

int fdc_obtenir(int fd, char *clef, char **valeur){

    return 0;
}

int fdc_iterateur(int fd, char **clef, char **valeur){

    char *chaineClef = NULL;
    char *chaineValeur = NULL;

    *clef = NULL;
    *valeur = NULL;

    if (fd == -1){
        fprintf(stderr, "le file descriptor est négatif (abandon)\n");
        return 0;
    }

    if (__lireJusuaDelimiteur(fd, &chaineClef, DELIMITEUR_CLEF_VALEUR)){
        *clef = chaineClef;

        if (__lireJusuaDelimiteur(fd, &chaineValeur, '\n')){
            *valeur = chaineValeur;
            return 1;
        }
    }

    return 0;

}

int fdc_iterateur2(int fd, char **clef, char **valeur, int delimiteur){

    int vieuxDel = fdc_delimiteur();
    fdc_changerDelimiteur(delimiteur);
    fdc_iterateur(fd,clef,valeur);
    fdc_changerDelimiteur(vieuxDel);
    return 0;

}
