#include "raylib.h"
#include "ennemi.h"
#include "cmath"

#ifndef ARME_H
#define ARME_H

class Arme
{
    public:
    int largeurEcran;
    int hauteurEcran;
    Camera *camera;

    int numeroArme;

    bool unlocked[9] = {true, true, true, true, true, true, true, true, true};
    int listeDegats[9] = {30, 20, 30, 60, 100*10, 30*10};
    float listePortees[9] = {0.8f, 1.2f, 5.0f, 1.5f, 2.0f, 8.0f};
    float listeAngles[9] = {PI/6.0f, PI/6.0f, 0.0f, PI/6.0f, PI/6.0f, 0.0f,};
    int maxTargets[9] = {1, 3, 1, 3, 6, 1, 100, };
    double reloadTime[9] = {0, 0, 1.5, 2.5, -1, 0, 3, 4};
    int clipAmmo[9] = {0, -1, 13, 5, 2, -1, 1, 666, 666};
    int clip[9]     = {0, 0, 13, 5, 2, 999, 1, 666, 666};
    int ammo[9]     = {0, 999, 999, 999, 999, 999, 999, 999, 999};




    //variables des animations
    bool animTir;
    int *frameCounter;
    int previousFrame;
    double previousTime;

    Texture2D weaponAtlas;
    
    Texture2D framesFists[4];

    Texture2D logoChainsaw;
    Texture2D framesChainsaw[4];

    Texture2D logoGun;
    Texture2D framesGun[6];

    Texture2D logoSpas12;
    Texture2D framesSpas12[6];

    Texture2D logoChasseur;
    Texture2D framesChasseur[10];

    Texture2D logoMinigun;
    Texture2D framesMinigun[4];

    Texture2D logoRPG;
    Texture2D framesRPG[6];

    Texture2D logoPlasma;
    Texture2D framesPlasma[4];

    Texture2D logoBFG;
    Texture2D framesBFG[5];

    int animFrame;

    Ennemi *listeEnnemis;
    int nbEnnemis;

    void Init(int largeurEcran, int hauteurEcran, int *frameCounter, Ennemi *listeEnnemisn,
              int nbEnnemis, Camera *camera);
    

    void Action();
    bool reloadPressed;
    bool mouseDown;
    bool mousePressed;
    bool reloadEnCours;

    void Render();
    void RenderReload(double startTime, double length);
    double reloadLength;
    double reloadStart;

    void RenderReticule(float range);
    Texture2D reticuleIn, reticuleOut;
    
    void GetWeaponTextures();

    void UpdateArme(int *numerosFrameAnimation, int *dureeFrames, Texture2D *weapon, int totalFrames);
    Texture2D toBeRendered;

    void Fire(int damage, float rangee, float width, int maxTargets);
    bool fire = true;
    bool powered = false;

    void ActionFists();
    void ActionChainsaw();
    void ActionGun();
    void ActionSpas12();
    void ActionChasseur();
    void ActionMinigun();
    void ActionRPG();
    void ActionPlasma();
    void ActionBFG();

    void CheckSwitchArme();
    void SwitchArmeAnimation();
    bool switchEnCours;
    int nextNumero = 0;
    float switchDecal;
    bool goUp;

    private:
};

class GunSpecs{
    int clipAmmo;
    int clipSize;
    int totalAmmo;
    int damage;
    float range;
    float width;
};

#endif