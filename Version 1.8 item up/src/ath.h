#include "raylib.h"
#include "arme.h"
#include "item.h"

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
    Texture2D instaKill ; 
    Texture2D gameOverScreen ; 
    Texture2D amogusScreen ; 
    Texture2D player;

    void Init(int largeurEcran, int hauteurEcran, Arme *arme, int *pvJoueur, int *nbKill);
    void Render();
    void gameOverRender();
    void startRender();

    private:
    int *pvJoueur;
};


#endif