#ifndef __CHAINE_ALLOCATION_H__
#define __CHAINE_ALLOCATION_H__ 1

#define chaineAffiche(chaine) (printf("%s\n", chaine))

#define chaineCopie(chaine) (strdup(chaine))

#define chaineContientCaractere(chaine,car) ((index((chaine), ((int)(car))) != NULL )? 1: 0)

#define pcalloc(var, taille, type) (type*)calloc(taille,sizeof(type));\
if(var == NULL) fprintf(stderr,"Erreur de malloc %s:%d (abandon)\n", __FILE__, __LINE__),exit(127);

/** ALLOUE et retourne une chaine contenant tout les chaines dans l'ordre passé en arguments **/
char* chaineFusion(int, char *chaine, ...);

/** afficheur **/
void chaineAfficheJusqua(char* chaine, char sep);
void afficherTableauDeString(char **cc);

/* compare c1 avec c2 et s'arréte quand c1[i] = finc1 
 * return 1 si c1 = c2 , 0 sinon*/
int comparerJusqua(char *c1, char *c2, char finc1);

#endif
