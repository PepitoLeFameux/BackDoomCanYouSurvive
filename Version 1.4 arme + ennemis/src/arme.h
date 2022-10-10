#include "raylib.h"
#include "ennemi.h"

#ifndef ARME_H
#define ARME_H

class Arme
{
    public:
    int largeurEcran;
    int hauteurEcran;
    Camera *camera;

    int ammo;
    int damage;
    float range;

    double time;
    double previousTime;
    bool verifTir; //variable pour savoir si le joueur peut tirer (cadence)

    //variables des animations
    bool animTir;
    int *frameCounter;
    int previousFrame;
    int frameSpeed;
    
    Texture2D weapon[4];
    int currentFrame;
    Texture2D reticule;

    Ennemi *listeEnnemis;
    int nbEnnemis;

    void Init(int largeurEcran, int hauteurEcran, int *frameCounter, Ennemi *listeEnnemisn,
              int nbEnnemis, Camera *camera);
    void Recharger();
    void Tirer();
    void Render();

    private:
};

#endif