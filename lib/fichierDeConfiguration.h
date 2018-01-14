#ifndef __FICHIER_DE_CONFIGURATION_H__
#define __FICHIER_DE_CONFIGURATION_H__ 1

#define NOMBRE_DE_CARACTERE_EN_EXTENTION 10

int fdc_ouvrir(char *chemin);
int fdc_fermer(int fd);
int fdc_obtenir(int fd, char *clef, char **valeur);
int fdc_iterateur(int fd, char **clef, char **valeur);
int fdc_iterateur2(int fd, char **clef, char **valeur, int delimiteur);
void fdc_changerDelimiteur(char nouv_del);
int fdc_delimiteur();

#endif