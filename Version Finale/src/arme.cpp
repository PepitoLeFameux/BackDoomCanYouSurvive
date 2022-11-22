#include "arme.h"
#include "raylib.h"
#include "raymath.h"
#include "string"
#include "cmath"
#include "iostream"
#include "ennemi.h"
#include "projectile.h"
#include "unordered_map"

void Arme::Init(int largeurEcran, int hauteurEcran, int* frameCounter, std::vector<Ennemi> *listeEnnemis,
                Camera *camera, Projectile *listeProjectiles, int nbProjectilesMax,
                std::unordered_map<std::string, bool> *effects, int *ammo, int *clip, int *clipAmmo,
                bool *unlocked)
{
    Arme::largeurEcran = largeurEcran;
    Arme::hauteurEcran = hauteurEcran;
    Arme::frameCounter = frameCounter;
    Arme::listeEnnemis = listeEnnemis;
    Arme::camera = camera;
    Arme::listeProjectiles = listeProjectiles;
    Arme::nbProjectilesMax = nbProjectilesMax;
    Arme::effects = effects;
    Arme::ammo = ammo;
    Arme::clip = clip;
    Arme::clipAmmo = clipAmmo;
    Arme::unlocked = unlocked;
    clipAmmo[1] = ammo[1];
    clipAmmo[4] = ammo[4];
    animFrame = 0;
    previousFrame = *frameCounter;
    previousTime = GetTime();
    animTir = false;
    numeroArme = 0;
    nextNumero = 0;
    playSound = 0 ;
    weaponAtlas = LoadTexture("../resources/weapons/weaponAtlas.png");
    reticuleIn = LoadTexture("../resources/weapons/reticlein.png");
    reticuleOut = LoadTexture("../resources/weapons/reticleout.png");
    gunShotSound = LoadSound("../resources/sound/gunshot.wav");
    shotgunSound = LoadSound("../resources/sound/shotgun.wav");
    chasseurSound = LoadSound("../resources/sound/chasseur.wav");
    rocketSound = LoadSound("../resources/sound/rocketlauncher.wav");
    ChainsawOff = LoadSound("../resources/sound/chainsawOff.wav");
    ChainsawOn = LoadSound("../resources/sound/chainsawOn.wav");
    minigunOn = LoadSound("../resources/sound/minigunOn.wav");
    minigunStart = LoadSound("../resources/sound/minigunStart.wav");


    GetWeaponTextures();
}

void Arme::CheckSwitchArme()
{
    int wheelMovement = GetMouseWheelMove();
    if (wheelMovement && !animTir && !reloadEnCours && !switchEnCours)
    {
        switchEnCours = true;
        switchDecal = 0.0f;
        nextNumero = 0;
        if(wheelMovement <= -1)
            for(int n = 1; n<10; n++) if (unlocked[(numeroArme + n)%7]) {nextNumero = (numeroArme + n)%7; break;}
        if(wheelMovement >= 1)
            for(int n = 1; n<10; n++) if (unlocked[(numeroArme - n + 7)%7]) {nextNumero = (numeroArme - n + 7)%7; break;}
    }
    if(switchEnCours){ SwitchArmeAnimation(); StopSound(ChainsawOff) ; StopSound(minigunOn) ; StopSound(ChainsawOn);}
}

void Arme::SwitchArmeAnimation()
{
    float switchTime = 1.0f;
    if(!goUp) switchDecal += 500.0f/30.0f*switchTime;
    else switchDecal -= 500.0f/30.0f*switchTime;
    if(switchDecal >= 500.0f) {goUp = true; numeroArme = nextNumero; animFrame = 0;}
    if(switchDecal <= 0.0f){switchDecal = 0.0f; goUp = false; switchEnCours = false;}
}


void Arme::Action()
{
    reloadPressed = IsKeyPressed(KEY_R);
    mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    CheckSwitchArme();

    if(numeroArme == 0) ActionFists();
    if(numeroArme == 1) ActionChainsaw();
    if(numeroArme == 2) ActionGun();
    if(numeroArme == 3) ActionSpas12();
    if(numeroArme == 4) ActionChasseur();
    if(numeroArme == 5) ActionMinigun();
    if(numeroArme == 6) ActionRPG();
    // if(numeroArme == 7) ActionPlasma();
    // if(numeroArme == 8) ActionBFG();
}

void Arme::ActionFists()
{
    int numeroFramesAnimation[] = {0, 1, 2};
    int dureeFrames[] = {0, 3, 12};
    int totalFrames = sizeof(numeroFramesAnimation)/sizeof(int);

    int frameDisplayed = numeroFramesAnimation[animFrame];
    toBeRendered = framesFists[frameDisplayed];

    if (animFrame == 2 && fire && !switchEnCours)
    {
        Fire(listeDegats[numeroArme], listePortees[numeroArme], listeAngles[numeroArme], maxTargets[numeroArme]);
        fire = false;
    }
        
    if ((animTir && *frameCounter - previousFrame >= dureeFrames[animFrame]) || mousePressed && !switchEnCours)
    {
        animTir = true;
        previousFrame = *frameCounter;
        animFrame = (animFrame + 1) % totalFrames;
        if(animFrame == 0) {animTir = false; fire = true;}
    }
}

void Arme::ActionChainsaw()
{
    int numeroFramesAnimation[] = {0, 1, 2, 3};
    int dureeFrames[] = {10, 10, 5, 5};
    int totalFrames = sizeof(numeroFramesAnimation)/sizeof(int);

    int frameDisplayed = numeroFramesAnimation[animFrame];
    toBeRendered = framesChainsaw[frameDisplayed];

    if (not(powered) && *frameCounter - previousFrame >= dureeFrames[animFrame])
    {
        StopSound(ChainsawOn);
        if(IsSoundPlaying(ChainsawOff) == false) PlaySound(ChainsawOff);
        previousFrame = *frameCounter;
        animFrame = (animFrame == 0)? 1 : 0;
    }

    else if (powered && *frameCounter - previousFrame >= dureeFrames[animFrame] && !switchEnCours)
    {
        previousFrame = *frameCounter;
        animFrame = (animFrame == 2)? 3 : 2;
        Fire(listeDegats[numeroArme], listePortees[numeroArme], listeAngles[numeroArme], maxTargets[numeroArme]);
        EnleverMunition();
        ammo[numeroArme] = clipAmmo[numeroArme];
        StopSound(ChainsawOff);
        if(IsSoundPlaying(ChainsawOn) == false) PlaySound(ChainsawOn);
    }
    
    clipAmmo[numeroArme] = ammo[numeroArme];

    if((mouseDown || mousePressed) && clipAmmo[numeroArme] > 0 && !switchEnCours)
        powered = true;
    else
        powered = false;
}

void Arme::ActionGun()
{
    int numeroFramesAnimation[] = {0, 1, 2, 3};
    int dureeFrames[] = {0, 3, 3, 3};
    int totalFrames = sizeof(numeroFramesAnimation)/sizeof(int);

    int frameDisplayed = numeroFramesAnimation[animFrame];
    toBeRendered = framesGun[frameDisplayed];

    if (animFrame == 1 && fire)
    {
        Fire(listeDegats[numeroArme], listePortees[numeroArme], listeAngles[numeroArme], maxTargets[numeroArme]);
        fire = false;
        PlaySound(gunShotSound);
    }
        
    if ((animTir && *frameCounter - previousFrame >= dureeFrames[animFrame]) || (mousePressed && clipAmmo[numeroArme] > 0 && not(reloadEnCours)) && !switchEnCours)
    {
        animTir = true;
        previousFrame = *frameCounter;
        animFrame = (animFrame + 1) % totalFrames;
        if(animFrame == 0) {animTir = false; EnleverMunition(); fire = true;}
    }

    else if((reloadPressed || reloadEnCours ) && clip[numeroArme] > clipAmmo[numeroArme] && ammo[numeroArme] > 0 && !switchEnCours)
    {
        if(not(reloadEnCours))
        {
            reloadStart = GetTime();
            reloadLength = reloadTime[numeroArme];
            reloadEnCours = true;
        }

        if(GetTime() - reloadStart > reloadLength)
        {
            while(ammo[numeroArme] > 0 && clipAmmo[numeroArme] < clip[numeroArme])
            {
                ammo[numeroArme] --;
                clipAmmo[numeroArme] ++;
            }
            reloadEnCours = false;
        }
    }
}

void Arme::ActionSpas12()
{
    int numeroFramesAnimation[] = {0, 1, 2, 3, 4, 5, 4, 3};
    int dureeFrames[] = {0, 3, 3, 6, 6, 6, 6, 6};
    int totalFrames = sizeof(numeroFramesAnimation)/sizeof(int);

    int frameDisplayed = numeroFramesAnimation[animFrame];
    toBeRendered = framesSpas12[frameDisplayed];

    if (animFrame == 1 && fire)
    {
        Fire(listeDegats[numeroArme], listePortees[numeroArme], listeAngles[numeroArme], maxTargets[numeroArme]);
        fire = false;
        PlaySound(shotgunSound);
    }
        
    if ((animTir && *frameCounter - previousFrame >= dureeFrames[animFrame]) || (mousePressed && clipAmmo[numeroArme] > 0 && not(reloadEnCours)) && !switchEnCours)
    {
        animTir = true;
        previousFrame = *frameCounter;
        animFrame = (animFrame + 1) % totalFrames;
        if(animFrame == 0) {animTir = false; EnleverMunition(); fire = true;}
    }

    else if((reloadPressed || reloadEnCours ) && clip[numeroArme] > clipAmmo[numeroArme] && ammo[numeroArme] > 0 && !switchEnCours)
    {
        if(not(reloadEnCours))
        {
            reloadStart = GetTime();
            reloadLength = reloadTime[numeroArme];
            reloadEnCours = true;
        }

        if(GetTime() - reloadStart > reloadLength)
        {
            while(ammo[numeroArme] > 0 && clipAmmo[numeroArme] < clip[numeroArme])
            {
                ammo[numeroArme] --;
                clipAmmo[numeroArme] ++;
            }
            reloadEnCours = false;
        }
    }
}

void Arme::ActionChasseur()
{
    int numeroFramesAnimation[] = {0, 1, 2, 3, 4, 3, 5};
    int dureeFrames[] = {0, 3, 3, 6, 6, 6, 6};
    int numeroFramesReload[] = {5, 6, 7, 8, 9};
    int dureeFramesReload[] = {20, 20, 20, 20, 20};
    int totalFrames = sizeof(numeroFramesAnimation)/sizeof(int);

    int frameDisplayed = reloadEnCours? numeroFramesReload[animFrame] : numeroFramesAnimation[animFrame];
    toBeRendered = framesChasseur[frameDisplayed];

    if (animFrame == 1 && fire && !reloadEnCours && !switchEnCours)
    {
        Fire(listeDegats[numeroArme], listePortees[numeroArme], listeAngles[numeroArme], maxTargets[numeroArme]);
        fire = false;
        PlaySound(chasseurSound);
    }
        
    if ((animTir && *frameCounter - previousFrame >= dureeFrames[animFrame]) || (mousePressed && clipAmmo[numeroArme] > 0 && not(reloadEnCours)) && !switchEnCours)
    {
        animTir = true;
        previousFrame = *frameCounter;
        animFrame = (animFrame + 1) % totalFrames;
        if(animFrame == 0) {animTir = false; EnleverMunition(); fire = true;}
    }

    else if(reloadEnCours || (reloadPressed && fire && clip[numeroArme] > clipAmmo[numeroArme] && ammo[numeroArme] > 0) && !switchEnCours)
    {
        if(not(reloadEnCours))
        {
            reloadStart = GetTime();
            reloadLength = 80.0/60.0;
            reloadEnCours = true;
            animFrame = 0;
        }

        if(*frameCounter - previousFrame >= dureeFramesReload[animFrame])
        {
            animFrame = (animFrame + 1) % 5;
            previousFrame = *frameCounter;
            if(animFrame == 0)
            {
                reloadEnCours = false;
                fire = true;
                while(ammo[numeroArme] > 0 && clipAmmo[numeroArme] < clip[numeroArme])
                {
                    ammo[numeroArme] --;
                    clipAmmo[numeroArme] ++;
                }
            }
        }
    }
}

void Arme::ActionMinigun()
{
    int numeroFramesAnimation[] = {0, 1, 2, 3};
    int dureeFrames[] = {10, 10, 3, 3};
    int totalFrames = sizeof(numeroFramesAnimation)/sizeof(int);

    int frameDisplayed = numeroFramesAnimation[animFrame];
    toBeRendered = framesMinigun[frameDisplayed];

    if (not(powered) && *frameCounter - previousFrame >= dureeFrames[animFrame])
    {
        StopSound(minigunOn);

        previousFrame = *frameCounter;
        animFrame = (animFrame == 0)? 1 : 0;
    }

    else if (powered && *frameCounter - previousFrame >= dureeFrames[animFrame] && !switchEnCours)
    {
        previousFrame = *frameCounter;
        animFrame = (animFrame == 2)? 3 : 2;
        Fire(listeDegats[numeroArme], listePortees[numeroArme], listeAngles[numeroArme], maxTargets[numeroArme]);
        EnleverMunition();
        ammo[numeroArme] = clipAmmo[numeroArme];
        if(IsSoundPlaying(minigunOn) == false) PlaySound(minigunOn);
    }

    if((mouseDown || mousePressed) && clipAmmo[numeroArme] > 0 && !switchEnCours)
        powered = true;
    else
        powered = false;
}

void Arme::ActionRPG()
{
    int numeroFramesAnimation[] = {0, 1, 2, 3, 4, 5};
    int dureeFrames[] = {0, 6, 6, 6, 6, 16};
    int totalFrames = sizeof(numeroFramesAnimation)/sizeof(int);

    int frameDisplayed = numeroFramesAnimation[animFrame];
    toBeRendered = framesRPG[frameDisplayed];

    if (animFrame == 1 && fire)
    {
        LaunchProjectile(camera->position, camera->target, "rocket");
        fire = false;
        PlaySound(rocketSound);
    }
        
    if ((animTir && *frameCounter - previousFrame >= dureeFrames[animFrame]) || (mousePressed && clipAmmo[numeroArme] > 0 && not(reloadEnCours)) && !switchEnCours)
    {
        animTir = true;
        previousFrame = *frameCounter;
        animFrame = (animFrame + 1) % totalFrames;
        if(animFrame == 0) {animTir = false; EnleverMunition(); fire = true;}
    }

    else if((reloadPressed || reloadEnCours ) && clip[numeroArme] > clipAmmo[numeroArme] && ammo[numeroArme] > 0 && !switchEnCours)
    {
        if(not(reloadEnCours))
        {
            reloadStart = GetTime();
            reloadLength = reloadTime[numeroArme];
            reloadEnCours = true;
        }

        if(GetTime() - reloadStart > reloadLength)
        {
            while(ammo[numeroArme] > 0 && clipAmmo[numeroArme] < clip[numeroArme])
            {
                ammo[numeroArme] --;
                clipAmmo[numeroArme] ++;
            }
            reloadEnCours = false;
        }
    }
}

void Arme::EnleverMunition()
{
    if(!(*effects)["infiniteammo"])
    {   
        clipAmmo[numeroArme] --;
    }
}

void Arme::Fire(int damage, float range, float angle, int maxTargets)
{
    Vector2 pointCollision;
    int targetCount = 0;
    for(int n=listeEnnemis->size()-1; n>=0; n--)
    {
        Ennemi *ennemi = &((*listeEnnemis)[n]);
        float dist = sqrt(pow(camera->position.x - ennemi->position.x, 2) + pow(camera->position.z - ennemi->position.z, 2));
        Vector3 vec3 = Vector3Add(camera->position, Vector3Scale(Vector3Normalize(Vector3Subtract(camera->target, camera->position)), range));
        if(ennemi->VisionDirecte(camera->position) &&
            ennemi->CheckCollisionLineCircle((Vector2){camera->position.x, camera->position.z},
                                            (Vector2){vec3.x, vec3.z},
                                            (Vector2){ennemi->position.x, ennemi->position.z},
                                            ennemi->taille/6.0f + dist * tanf(angle/2.0f), &pointCollision) &&
           !(ennemi->dead))
        {
            ennemi->Damaged(((*effects)["instakill"])? 9999 : damage);
            targetCount ++;
            if(targetCount >= maxTargets)
                break;
        }
    }
}

int Arme::FindNewSlot()
{
    for(int n=0; n< nbProjectilesMax; n++)
    {
        if(!listeProjectiles[n].active) return n;
    }
}

void Arme::LaunchProjectile(Vector3 position, Vector3 direction, std::string type)
{
    int slot = FindNewSlot();
    listeProjectiles[slot].Launch(camera->position, camera->target, type);
}

void Arme::GetWeaponTextures()
{
    std::string nomsArmes[] = {"fists", "chainsaw", "gun", "spas12", "chasseur", "minigun", "rpg"};
    Texture2D *listesFrames[] = {framesFists, framesChainsaw, framesGun, framesSpas12, framesChasseur, framesMinigun, framesRPG};
    int m, n;
    for(m=0; m < *(&nomsArmes + 1) - nomsArmes; m++)
    {
        n=0;
        while (FileExists(((std::string)("../resources/weapons/" + nomsArmes[m] + "-" + std::to_string(n) + ".png")).c_str()))
        {
            listesFrames[m][n] = LoadTexture(((std::string)("../resources/weapons/" + nomsArmes[m] + "-" + std::to_string(n) + ".png")).c_str());
            n++;
        }
    }
}

void Arme::Render()
{
    if(numeroArme == 4)
    {
        DrawTexturePro(toBeRendered, (Rectangle){0.0f, 0.0f, (float)toBeRendered.width, (float)toBeRendered.height},
                (Rectangle){(float)largeurEcran/2.0f, (float)hauteurEcran/2.0f + switchDecal,
                            (float)largeurEcran, (float)hauteurEcran},
                (Vector2){(float)largeurEcran/2.0f,(float)hauteurEcran/2.0f},
                0.0f, WHITE);
        if(reloadEnCours) RenderReload(reloadStart, reloadLength);
    }
    else{
        if(reloadEnCours)
            RenderReload(reloadStart, reloadLength);
        else
            DrawTexturePro(toBeRendered, (Rectangle){0.0f, 0.0f, (float)toBeRendered.width, (float)toBeRendered.height},
                    (Rectangle){(float)largeurEcran/2.0f, (float)hauteurEcran/2.0f + switchDecal,
                                (float)largeurEcran, (float)hauteurEcran},
                    (Vector2){(float)largeurEcran/2.0f,(float)hauteurEcran/2.0f},
                    0.0f, WHITE);
    }
    RenderReticule(listePortees[numeroArme], listeAngles[numeroArme]);
}

void Arme::RenderReload(double startTime, double length)
{
    std::string timeLeft = std::to_string(std::ceil((length - GetTime() + startTime)*10)/10);
    timeLeft.erase(timeLeft.find_last_not_of("0") + 1, std::string::npos);
    DrawText((timeLeft + "s").c_str(), 720*largeurEcran/1080, 570*hauteurEcran/720, 50, WHITE);
    DrawRectangleLinesEx((Rectangle){390*largeurEcran/1080, 570*hauteurEcran/720, 300*largeurEcran/1080, 50*hauteurEcran/720}, 10.0f, WHITE);
    DrawRectangle(390*largeurEcran/1080, 570*hauteurEcran/720, 300*largeurEcran/1080*(GetTime() - startTime)/length, 50*hauteurEcran/720, WHITE);
}

void Arme::RenderReticule(float range, float angle)
{
    Vector2 pointCollision;
    bool ennemiDetecte = false;
    for(int n=0; n<listeEnnemis->size(); n++)
    {
        Ennemi *ennemi = &((*listeEnnemis)[n]);
        float dist = sqrt(pow(camera->position.x - ennemi->position.x, 2) + pow(camera->position.z - ennemi->position.z, 2));
        Vector3 vec3 = Vector3Add(camera->position, Vector3Scale(Vector3Normalize(Vector3Subtract(camera->target, camera->position)), range));
        if(ennemi->VisionDirecte(camera->position) &&
            ennemi->CheckCollisionLineCircle((Vector2){camera->position.x, camera->position.z},
                                            (Vector2){vec3.x, vec3.z},
                                            (Vector2){ennemi->position.x, ennemi->position.z},
                                            ennemi->taille/6.0f + dist * tanf(angle/2.0f), &pointCollision) &&
           !(ennemi->dead))
        {
            ennemiDetecte = true;
            break;
        }
    }

    float c = 40.0f;
    DrawTexturePro(reticuleOut, (Rectangle){0, 0, reticuleOut.width, reticuleOut.height}, 
                   (Rectangle){(largeurEcran-c)/2.0f, (hauteurEcran-c)/2.0f, c, c},
                   Vector2Zero(), 0.0f, BLACK);
    DrawTexturePro(reticuleIn, (Rectangle){0, 0, reticuleIn.width, reticuleIn.height}, 
                   (Rectangle){(largeurEcran-c)/2.0f, (hauteurEcran-c)/2.0f, c, c},
                   Vector2Zero(), 0.0f, (ennemiDetecte)? RED : BLACK);
}