#include "raylib.h"
#include "arme.h"

#ifndef ACHAT_H
#define ACHAT_H

class Achat
{
    public:
    void Init(Camera *camera, Arme *arme, bool *unlocked, int *argentn, int *ammo, int *getAmmo);
    void Render();
    void RenderMessage();
    void Acheter();

    private :
    int *argent ;
    int *ammo;
    int *getAmmo;
    bool *unlocked ; 
    int distJoueurListe[6] ;

    Arme *arme ;
    Camera *camera ;
    Sound cashRegister ; 

    Texture2D weapon[6] ;
    Rectangle rec_src[6] ; 
    bool achatPossibleListe[6] = {false, false, false, false, false, false};
    Vector3 positionAchatListe[6] = {{1.0f,0.3f,1.0f},{},{0.0f,0.3f,5.0f},{},{0.0f,0.3f,3.0f},{2.0f,0.3f,3.0f}};
    int prixListe[6] = {1500,100,750,3500,5000,7000} ; 
};


#endif