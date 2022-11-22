#include "raylib.h"
#include "cmath"
#include "unordered_map"
#include "string"
#include "vector"

#ifndef ARME_H
#define ARME_H

class Projectile;
class Ennemi;

class Arme
{
    public:
    int largeurEcran;
    int hauteurEcran;
    Camera *camera;
    Projectile *listeProjectiles;
    int nbProjectilesMax;

    int numeroArme;
    std::unordered_map<std::string, bool> *effects;

    bool *unlocked;

    //poing Chainsaw Gun Shotgun Chasseur Minigun Rocket .......

    int listeDegats[9] = {30, 20, 30, 60, 100, 30, 300};
    float listePortees[9] = {1.0f, 1.3f, 5.0f, 1.5f, 2.0f, 8.0f, 100.0f};
    float listeAngles[9] = {PI/6.0f, PI/6.0f, 0.0f, PI/6.0f, PI/6.0f, 0.0f, -1.0f};
    int maxTargets[9] = {1, 3, 1, 3, 6, 1, -1};
    double reloadTime[9] = {0, 0, 1.5, 2.5, -1, 5, 4};
    int *clipAmmo;
    int *clip;
    int *ammo;
    int playSound ;

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

    Sound gunShotSound ; 
    Sound shotgunSound ;
    Sound chasseurSound ; 
    Sound rocketSound ;
    Sound ChainsawOff ;
    Sound ChainsawOn;
    Sound ChainsawStart ;
    Sound minigunOn ;
    Sound minigunStart ;

    int animFrame;

    std::vector<Ennemi> *listeEnnemis;

    void Init(int largeurEcran, int hauteurEcran, int *frameCounter, std::vector<Ennemi> *listeEnnemisn, Camera *camera, Projectile *listeProjectiles, int nbProjectilesMax,
              std::unordered_map<std::string, bool> *effects, int *ammo, int *clip, int *clipAmmo,
              bool *unlocked);
    

    void Action();
    bool reloadPressed;
    bool mouseDown;
    bool mousePressed;
    bool reloadEnCours;

    void Render();
    void RenderReload(double startTime, double length);
    double reloadLength;
    double reloadStart;

    void RenderReticule(float range, float angle);
    Texture2D reticuleIn, reticuleOut;
    
    void GetWeaponTextures();

    void UpdateArme(int *numerosFrameAnimation, int *dureeFrames, Texture2D *weapon, int totalFrames);
    Texture2D toBeRendered;

    void Fire(int damage, float rangee, float width, int maxTargets);
    bool fire = true;
    bool powered = false;

    void LaunchProjectile(Vector3 position, Vector3 direction, std::string type);

    void ActionFists();
    void ActionChainsaw();
    void ActionGun();
    void ActionSpas12();
    void ActionChasseur();
    void ActionMinigun();
    void ActionRPG();
    void ActionPlasma();
    void ActionBFG();
    void EnleverMunition();

    void CheckSwitchArme();
    void SwitchArmeAnimation();
    bool switchEnCours;
    int nextNumero = 0;
    float switchDecal;
    bool goUp;

    int FindNewSlot();
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