#ifndef __MEMOIRE_PARTAGER_H__
#define __MEMOIRE_PARTAGER_H__ 1

#ifndef ERR 
#define ERR -1
#endif

#define MEMOIRE_PARTAGER_SEPARATEUR '\n'
#define TAILLE_MEMOIRE_PARTAGER_DEFAUT 2048

#define MEMP_ERREUR_KEYT "key_t-1\n"
#define MEMP_ERREUR_SHMGET "shmget-1\n"
#define MEMP_ERREUR_SHMAT "shmat-1\n"

#define MEMP_ERREUR_ELEMENT_NON_TROUVER "Element non trouvé\n"
#define MEMP_ERREUR_INCOMPLETE "Commande incompléte (utiliser %s <variable>=<valeur>)\n"
#define MEMP_ERREUR_INCORRECT "la variable doit être initialisé  (utiliser %s <variable>=<valeur>)\n"
#define MEMP_ERREUR_PASASSEZDESPACE "Espace insufisant dans le zone pour inserer [%s] (abandon)\n"

#define MP_CREE(clef, s) (shmget(clef, s, IPC_CREAT | 0600))
#define MP_MAT(r,t,id) (r=(t)shmat(id,0,0))

typedef int MemoirePartagerId;

typedef struct {

    char buffer[TAILLE_MEMOIRE_PARTAGER_DEFAUT];
    int  stouage[TAILLE_MEMOIRE_PARTAGER_DEFAUT];
    int  acces;

} ZoneMp;

void* __mps_attacher(MemoirePartagerId id);
void __mps_detruireLesSemaphores(MemoirePartagerId id);
void __mps_detacher(void *zone);
int  __mps_emplacementvide(char *buffer, int max);
int  __mps_nouvelleEntrer(int *stouage, int empl);
int  __mps_sub(int *stouage, char *buffer, int bg, int bd, int tsc, char *clefval, int cvlen);
int  __mps_obtenirp(char *buffer, int *stouage, char *clef, int cleflen, int **empl);
int  __mps_supprimer(char *buffer, int *stouage, char *clef);
int  __mps_inserer(char *buffer, int *stouage, char *clefval);
int  __mps_obetnir(char *buffer, int *stouage, char *clef, char **resultat);
void __mps_afficher(char *buffer, int *stouage, void(*affichage)(char* c) );


MemoirePartagerId mps_nouveau(key_t clef, int creeNouveaux);
key_t mps_obtenirclef(char *unChemin, int unEntier);
void mps_vider(MemoirePartagerId id);
void mps_detruire(MemoirePartagerId id);
int mps_ajouter(MemoirePartagerId id, char *clefvaleur);
int mps_enlever(MemoirePartagerId id, char *clefvaleur);
int mps_insererliste(MemoirePartagerId id, char **ensemble);
int mps_afficher(MemoirePartagerId id, void(*affichage)(char* c) );
int mps_estvide(MemoirePartagerId id);
int mps_obtenir(MemoirePartagerId id, char *clef, char **resultat);
int mps_bonformat(char *clefvaleur);

#endif