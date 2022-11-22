#include "raylib.h"
#include "arme.h"

#ifndef ACHAT_H
#define ACHAT_H

class Achat
{
    public:

    bool *unlocked ; 
    int distJoueurListe[6] ;

    Arme *arme ;
    Camera *camera ;
    Sound cashRegister ; 

//Chainsaw Gun Shotgun Chasseur Minigun Rocket .......
    Texture2D weapon[6] ;
    Rectangle rec_src[6] ; 
    bool achatPossibleListe[6] = {false, false, false, false, false, false};
    Vector3 positionAchatListe[6] = {{-23.0f,0.3f,-23.0f},{0.3f,0.3f,0.3f},{-9.8f,0.3f,14.4f},{8.5f,0.3f,2.4f},{-23.0f,0.3f,21.0f},{18.0f,0.3f,-12.0f}};
    int prixListe[6] = {1500,100,750,3500,5000,7000} ; 

    void Init(Camera *camera, Arme *arme, bool *unlocked, int *argent, int *ammo, int *getAmmo);
    void Render();
    void RenderMessage();
    void Acheter();

    private :
    int *argent ;
    int *ammo;
    int *getAmmo;
};





#endif