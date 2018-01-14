#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "memoirePartager.h"

struct sembuf SEM_MONTER = {0,1,0};
struct sembuf SEM_DECENDRE = {0,-1,0};

void V(int s){

    semop(s, &SEM_MONTER, 1);
}

void P(int s){

    semop(s, &SEM_DECENDRE, 1);
}

int mps_bonformat(char *clefvaleur){

    if (*clefvaleur == '=') return 0;
    for ( ; *clefvaleur && *clefvaleur != '='; ++clefvaleur);
    if (*clefvaleur != '=') return 0;
    ++clefvaleur;
    return *clefvaleur;
}

key_t mps_obtenirclef(char *unChemin, int unEntier){

    key_t k = ftok(unChemin,unEntier+1);
    if (k == ERR){ fprintf(stderr, MEMP_ERREUR_KEYT); exit(errno);}
    return k;
}

void mps_vider(MemoirePartagerId id){

    ZoneMp *zone = (ZoneMp*) __mps_attacher(id);

    memset(zone,0,sizeof(ZoneMp));
    memset(zone->stouage, -1, TAILLE_MEMOIRE_PARTAGER_DEFAUT);
    __mps_detacher(zone);
}

void __mps_detruireLesSemaphores(MemoirePartagerId id){

    ZoneMp *zone = (ZoneMp*) __mps_attacher(id);
    semctl(zone->acces, 0, IPC_RMID);
    __mps_detacher(zone);
}

MemoirePartagerId mps_nouveau(key_t clef, int creeNouveaux){

    MemoirePartagerId id;
    ZoneMp *table;

    if (creeNouveaux) id = MP_CREE(clef, sizeof(ZoneMp));
    else {
        id = shmget(clef, sizeof(ZoneMp), 0600); 
        return id; 
    }

    if (id == ERR) { fprintf(stderr, MEMP_ERREUR_SHMGET); exit(errno);}
    mps_vider(id);
    table = (ZoneMp*) __mps_attacher(id);
    table->acces = semget(IPC_PRIVATE, 1, 0600);
    semctl(table->acces, SETVAL, 1);
    __mps_detacher(table);
    return id;

}

void* __mps_attacher(MemoirePartagerId id){

    void* zone = NULL;

    zone = shmat(id, 0, 0);
    if (zone == NULL) { fprintf(stderr, MEMP_ERREUR_SHMAT); exit(errno);}
    return zone;
}

void __mps_detacher(void *zone){

    shmdt(zone);
}

void mps_detruire(MemoirePartagerId id){

    __mps_detruireLesSemaphores(id);
    shmctl(id, IPC_RMID, 0);
}

int __mps_emplacementvide(char *buffer, int max){

    int i=0;
    for (; i<max && !(buffer[i]); ++i);
    return i == max;
}

int __mps_nouvelleEntrer(int *stouage, int empl){

    for (; *stouage != -1; ++stouage);
    *stouage = empl; *(stouage+1) = -1;
    return 1;
}

int __mps_sub(int *stouage, char *buffer, int bg, int bd, int tsc, char *clefval, int cvlen){

    if (tsc < cvlen+1) return 0;

    if ( (tsc>>1) < cvlen+1 ){

        if (__mps_emplacementvide(buffer+bg, tsc) ){
            __mps_nouvelleEntrer(stouage, bg);
            strncpy(buffer+bg, clefval, cvlen);
            return 1;
        }

        else if (__mps_emplacementvide(buffer+bd, tsc) ){
            __mps_nouvelleEntrer(stouage, bd);
            strncpy(buffer+bd, clefval, cvlen);
            return 1;
        }

        else return 0;
    }


    tsc >>=1;
    return __mps_sub(stouage, buffer, bg, bd>>1, tsc, clefval, cvlen) ||
           __mps_sub(stouage, buffer, bd, bd+(bd>>1), tsc, clefval, cvlen);
}

int __mps_obtenirp(char *buffer, int *stouage, char *clef, int cleflen, int **empl){

    *empl = NULL;
    for (; *stouage != -1; ++stouage)
        if ( !strncmp( (buffer+(*stouage) ), clef, cleflen) && buffer[(*stouage)+cleflen] == '=')
             *empl = stouage; return 1;
    
    
    return 0;
}

int __mps_supprimer(char *buffer, int *stouage, char *clef){
    
    int clen = strlen(clef);
    int *empl = NULL;

    if (__mps_obtenirp(buffer, stouage, clef, clen, &empl) && empl){

        for (buffer += *empl; *buffer; ++buffer) *buffer = '\0';
        
        for (; *(empl+1) != -1; empl++) *empl = *(empl+1);
        *empl = -1;
        return 1;
    }

    return 0;
}

int __mps_inserer(char *buffer, int *stouage, char *clefval){
    
    char *c = clefval;
    for (; *c != '='; ++c);
    *c = '\0';

    __mps_supprimer(buffer, stouage, clefval); *c = '=';
    return (!__mps_sub(stouage, buffer, 0, TAILLE_MEMOIRE_PARTAGER_DEFAUT>>1,
              TAILLE_MEMOIRE_PARTAGER_DEFAUT>>1, clefval, strlen(clefval)))? -2 : 1;
}

int __mps_obetnir(char *buffer, int *stouage, char *clef, char **resultat){

    int clen = strlen(clef);
    int *empl = NULL;

    *resultat = NULL;
    if (__mps_obtenirp(buffer, stouage, clef, clen, &empl) && empl) *resultat = strdup(buffer+(*empl)+clen+1);
    return *resultat != NULL;
}

void __mps_afficher(char *buffer, int *stouage, void(*affichage)(char* c) ){

    for (; *stouage != -1 ; ++stouage ) affichage(buffer+(*stouage));

    putchar('\n');
    return ;
}
    
int mps_ajouter(MemoirePartagerId id, char *clefvaleur){

    int status = 0;
    
    if ( !mps_bonformat(clefvaleur) ) return 0;
    ZoneMp *zone = (ZoneMp*) __mps_attacher(id);
    P(zone->acces);
    status = __mps_inserer(zone->buffer,zone->stouage, clefvaleur);
    V(zone->acces);
    __mps_detacher(zone);
    return status;
    
}

int mps_enlever(MemoirePartagerId id, char *clefvaleur){
    
    int status = 0;
    
    ZoneMp *zone = (ZoneMp*) __mps_attacher(id);
    P(zone->acces);
    status = __mps_supprimer(zone->buffer,zone->stouage, clefvaleur);
    V(zone->acces);
    __mps_detacher(zone);
    return status;
}

int mps_insererliste(MemoirePartagerId id, char **ensemble){

    int code = 1;
    for (; *ensemble && (code=mps_ajouter(id, *ensemble)) == 1; ++ensemble);
    return code;
}

int mps_afficher(MemoirePartagerId id, void(*affichage)(char* c) ){

    ZoneMp *zone = (ZoneMp*) __mps_attacher(id);
    P(zone->acces);
    __mps_afficher(zone->buffer,zone->stouage,affichage);
    V(zone->acces); 
    __mps_detacher(zone);
    return 1;

}

int mps_estvide(MemoirePartagerId id){

    int status = 0;

    ZoneMp *zone = (ZoneMp*) __mps_attacher(id);
    P(zone->acces);
    status = *(zone->stouage) == -1;
    V(zone->acces);
    __mps_detacher(zone);
    return status;
}

int mps_obtenir(MemoirePartagerId id, char *clef, char **resultat){
    
    int status = 0;
    
    ZoneMp *zone = (ZoneMp*) __mps_attacher(id);
    P(zone->acces);
    status = __mps_obetnir(zone->buffer,zone->stouage, clef, resultat);
    V(zone->acces);
    __mps_detacher(zone);
    return status;
    
}