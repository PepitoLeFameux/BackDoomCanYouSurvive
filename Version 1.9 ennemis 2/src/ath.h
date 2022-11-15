#include "raylib.h"
#include "arme.h"
#include "unordered_map"

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
    Texture2D weapons[9];
    Texture2D amogusScreen;
    Texture2D gameOverScreen ;
    Texture2D instaKill ; 

    void Init(int largeurEcran, int hauteurEcran, Arme *arme, int *pvJoueur, int *shieldJoueur, int *nbKill,std::unordered_map<std::string, bool> *effects);
    void Render();
    void gameOverRender();

    private:
    int *pvJoueur;
    int *shieldJoueur;
    std::unordered_map<std::string, bool> *effects;
};


#endif