#include "raylib.h"
#include "arme.h"
#include "unordered_map"
#include "achat.h"

#ifndef ATH_H
#define ATH_H

class Ath
{
    public:
    Arme *arme ;

    int largeurEcran ;
    int hauteurEcran ;
    int *nbKill ;  
    unsigned __int8 *numManche ; 
    int temps ;
    int tempsPrec ; 

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
    Texture2D dollar ; 
    Texture2D backroom ;
    Texture2D miniMap ;
    Texture2D radar ;
    Texture2D ammoMax ;
    Texture2D logoSprint;
    Font police ;
    Camera *camera; 
    Font police2 ;
    Achat achat ;

    void Init(Camera *camera, int largeurEcran, int hauteurEcran, Arme *arme, int *pvJoueur, int *shieldJoueur, int *nbKill,std::unordered_map<std::string, bool> *effects, unsigned __int8 *numManche, int *argent, int *ennemisRestants, std::vector<Ennemi> *ennemis);
    void Render();
    void gameOverRender();
    void startRender();
    void displaySprint(int sprintJauge);

    private:
    int *pvJoueur;
    int *shieldJoueur;
    int *argent ; 
    int *ennemisRestants ;
    std::unordered_map<std::string, bool> *effects;
    std::vector<Ennemi> *ennemis;
};


#endif