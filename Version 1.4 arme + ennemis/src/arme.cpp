#include "arme.h"
#include "raylib.h"
#include "raymath.h"
#include "string"
#include "iostream"
#include "ennemi.h"

void Arme::Init(int largeurEcran, int hauteurEcran, int* frameCounter, Ennemi *listeEnnemis,
                int nbEnnemis, Camera *camera)
{
    ammo = 13;
    Arme::largeurEcran = largeurEcran;
    Arme::hauteurEcran = hauteurEcran;
    Arme::frameCounter = frameCounter;
    Arme::listeEnnemis = listeEnnemis;
    Arme::nbEnnemis = nbEnnemis;
    Arme::camera = camera;
    frameSpeed = 20;
    currentFrame = 0;
    previousFrame = *frameCounter;
    previousTime = GetTime();
    animTir = false;
    damage = 1000;
    range = 15.0f;
    for(int n=0; n<4; n++)
    {
        weapon[n] = LoadTexture(((std::string)("../resources/weapons/gun-frame" + std::to_string(n) + ".png")).c_str());
    }
    //reticule = LoadTexture("../resources/weapons/reticule.png");
}

void Arme::Recharger()
{
    ammo = 13 ;
}

void Arme::Tirer()
{
    time = GetTime();
    if ((time - previousTime) >= 0.3){
        if (ammo > 0)
        {
            previousTime = time;
            ammo --;
            animTir = true;
            Vector2 pointCollision;
            for(int n=0; n<nbEnnemis; n++)
            {
                Ennemi *ennemi = &listeEnnemis[n];
                if(ennemi->VisionDirecte(camera->position) &&
                   ennemi->CheckCollisionLineCircle((Vector2){camera->position.x, camera->position.z},
                                                      (Vector2){Vector3Lerp(camera->position, camera->target, range*10.0f/3.0f).x,
                                                                Vector3Lerp(camera->position, camera->target, range*10.0f/3.0f).z},
                                                      (Vector2){ennemi->position.x, ennemi->position.z},
                                                      ennemi->taille/6.0f, &pointCollision) &&
                    !(ennemi->dead))
                {
                    ennemi->Damaged(damage);
                    break;
                }
            }
        }
    } 
}

void Arme::Render()
{
    DrawTexturePro(weapon[currentFrame], (Rectangle){0.0f, 0.0f, (float)weapon[0].width, (float)weapon[0].height},
                   (Rectangle){(float)largeurEcran/2.0f, (float)hauteurEcran/2.0f,
                               (float)largeurEcran, (float)hauteurEcran},
                   (Vector2){(float)largeurEcran/2.0f,(float)hauteurEcran/2.0f},
                   0.0f, WHITE);  

    if (animTir && *frameCounter - previousFrame >= (60/frameSpeed)){ //Vitesse d'amination
            previousFrame = *frameCounter;
            currentFrame = (currentFrame + 1) % 4;
            if(currentFrame == 0)
                animTir = false;
        }
        
}