#include "projectile.h"
#include "raylib.h"
#include "raymath.h"
#include "iostream"
#include "ennemi.h"
#include "vector"

bool Projectile::CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center,
                                      float radius, Vector2 *pointCollision)
{
    float portions = (float) 15;
    Vector2 pos1, pos2;
    for(float alpha=0; alpha < 2*PI*(1.0f - 1.0f/portions); alpha += 2*PI/8.0f)
    {
        pos1.x = (float) center.x + radius*cos(alpha),             pos1.y = (float) center.y + radius*sin(alpha);
        pos2.x = (float) center.x + radius*cos(alpha + 2*PI/8.0f), pos2.y = (float) center.y + radius*sin(alpha + 2*PI/8.0f);
        if(CheckCollisionLines(pos1, pos2, startPos, endPos, pointCollision)) return true;
    }
    return false;
}

void Projectile::Init(std::vector<Ennemi> *listeEnnemis, Color *mapCouleurs, Texture2D dimensionsMap,
                      Vector3 mapPosition, Projectile *listeProjectiles, int nbProjectilesMax, int *frameCounter,
                      Texture2D *texturesRoquette, Camera *camera, int largeur, int hauteur)
{
    Projectile::listeEnnemis = listeEnnemis;
    Projectile::mapCouleurs = mapCouleurs;
    Projectile::dimensionsMap = dimensionsMap;
    Projectile::mapPosition = mapPosition;
    Projectile::listeProjectiles = listeProjectiles;
    Projectile::nbProjectilesMax = nbProjectilesMax;
    Projectile::frameCounter = frameCounter;
    Projectile::texturesRoquette = texturesRoquette;
    Projectile::camera = camera;
    Projectile::largeur = largeur;
    Projectile::hauteur = hauteur;
    active = false;
    explosionSound = LoadSound("../resources/sound/explosion.wav");
}

void Projectile::Launch(Vector3 position, Vector3 direction, std::string type)
{
    Projectile::direction = Vector3Subtract(direction, position);
    Projectile::position = Vector3Add(position, Vector3Scale(Vector3Normalize(Projectile::direction), 0.3f));
    active = true;
    detonated = false;
    animFrame = 0;
    previousFrame = *frameCounter;
    previousTime = GetTime();
    hit = false;
    if(type == "rocket")
    {
        speed = 3.0f;
        radius = 1.5f;
        taille = 1.5f;
        damage = 300;
        affiliation = "allied";
        dureeFrames = dureeFramesRoquette;
        totalFrames = totalFramesRoquette;
        textures = texturesRoquette;
    }
}

void Projectile::Action()
{
    if(active)
    {
        if(!hit)
        {
            position = (Vector3){Vector3Add(position, Vector3Scale(Vector3Normalize(direction), speed*(GetTime() - previousTime))).x, position.y, Vector3Add(position, Vector3Scale(Vector3Normalize(direction), speed*(GetTime() - previousTime))).z};
            
            CheckCollisionMurs();
            if(affiliation == "allied")
                CheckCollisionEnnemis();
            else{}
                // CheckCollisionJoueur();
        }
        if(hit)
        {
            if(!detonated){Detonate(); detonated = true; PlaySound(explosionSound);}
            else if(*frameCounter - previousFrame >= dureeFrames[animFrame])
            {
                previousFrame = *frameCounter;
                animFrame = (animFrame + 1)%totalFrames;
                if(animFrame == 0) active = false;
            }
        }
    }
}

void Projectile::CheckCollisionMurs()
{
    int caseX = (int)(position.x - mapPosition.x + 0.5f);
    int caseY = (int)(position.z - mapPosition.z + 0.5f);
    Vector2 pointCollision;

    if((mapCouleurs[(caseY-1)*dimensionsMap.width+caseX].r == 255
        && CheckCollisionLineCircle((Vector2){mapPosition.x + caseX*1.0f - 0.5f, mapPosition.z + caseY*1.0f - 0.5f},
                            (Vector2){mapPosition.x + caseX*1.0f + 0.5f, mapPosition.z + caseY*1.0f - 0.5f}, 
                            (Vector2){position.x, position.z},
                            taille/6.0f, &pointCollision)
        ) ||
        (mapCouleurs[(caseY+1)*dimensionsMap.width+caseX].r == 255
        && CheckCollisionLineCircle((Vector2){mapPosition.x + caseX*1.0f - 0.5f, mapPosition.z + caseY*1.0f + 0.5f},
                            (Vector2){mapPosition.x + caseX*1.0f + 0.5f, mapPosition.z + caseY*1.0f + 0.5f}, 
                            (Vector2){position.x, position.z},
                            taille/6.0f, &pointCollision)
        ) ||
        (mapCouleurs[caseY*dimensionsMap.width+caseX-1].r == 255
        && CheckCollisionLineCircle((Vector2){mapPosition.x + caseX*1.0f - 0.5f, mapPosition.z + caseY*1.0f - 0.5f},
                            (Vector2){mapPosition.x + caseX*1.0f - 0.5f, mapPosition.z + caseY*1.0f + 0.5f}, 
                            (Vector2){position.x, position.z},
                            taille/6.0f, &pointCollision)
        ) ||
        (mapCouleurs[caseY*dimensionsMap.width+caseX+1].r == 255
        && CheckCollisionLineCircle((Vector2){mapPosition.x + caseX*1.0f + 0.5f, mapPosition.z + caseY*1.0f - 0.5f},
                            (Vector2){mapPosition.x + caseX*1.0f + 0.5f, mapPosition.z + caseY*1.0f + 0.5f}, 
                            (Vector2){position.x, position.z},
                            taille/6.0f, &pointCollision)
        )
    )
    {
        hit = true;      
    }  
}

void Projectile::CheckCollisionEnnemis()
{
    for(int n = 0; n<listeEnnemis->size(); n++)
    {
        Ennemi ennemi = (*listeEnnemis)[n];
        float dist = Vector2Distance((Vector2){position.x, position.z}, (Vector2){ennemi.position.x, ennemi.position.z});
        if(!ennemi.dead && dist < ennemi.taille/6.0f + taille/6.0f) {hit = true; break;};
    }
}

void Projectile::Detonate()
{
    previousFrame = *frameCounter;
    for(int n = 0; n<listeEnnemis->size(); n++)
    {
        Ennemi ennemi = (*listeEnnemis)[n];
        float dist = Vector2Distance((Vector2){position.x, position.z}, (Vector2){ennemi.position.x, ennemi.position.z});
        if(dist < ennemi.taille/6.0f + radius) {(*listeEnnemis)[n].Damaged(damage);}
    }
}

void Projectile::Render()
{
    
    if(active)
    {
    Texture2D sprite = texturesRoquette[animFrame];
    DrawBillboardPro(*camera, sprite, (Rectangle){0, 0, sprite.width, sprite.height}, position,
                     (Vector3){0.0f, 1.0f, 0.0f},
                     (Vector2){(float)taille*sprite.width/171, (float)taille*sprite.height/186},
                     (Vector2){(float)sprite.width/2.0f, (float)sprite.height/2.0f}, 0.0f, WHITE);
    }
}