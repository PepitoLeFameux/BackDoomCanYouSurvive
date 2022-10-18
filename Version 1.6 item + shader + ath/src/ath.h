#include "raylib.h"
#include "arme.h"
#include "shieldObject.h"

#ifndef ATH_H
#define ATH_H

class Ath
{
    public:
    Arme *arme ;

    int largeurEcran ;
    int hauteurEcran ;
    int *nbKill ;  

    Texture2D cadreAthArmes ;
    Texture2D heart ; 
    Texture2D heart1 ;
    Texture2D heart2 ;
    Texture2D heart3 ;
    Texture2D shield ; 
    Texture2D player;

    void Init(int largeurEcran, int hauteurEcran, Arme *arme, int *pvJoueur, int *nbKill);
    void Render();

    private:
    int *pvJoueur;
};


#endif