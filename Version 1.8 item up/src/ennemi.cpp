#include "ennemi.h"
#include "raylib.h"
#include "cmath"
#include "string"
#include "iostream"
#include "item.h"
#include "string"

int Ennemi::tagEnnemi = 0;

void Ennemi::Init(Color *mapCouleurs, Texture2D dimensionsMap, Vector3 mapPosition, Camera *camera,
                  int *pvJoueur, Texture2D *listeTextures, int *shieldJoueur, int *shieldJoueurMax)
{
    Ennemi::mapCouleurs = mapCouleurs;
    Ennemi::dimensionsMap = dimensionsMap;
    Ennemi::mapPosition = mapPosition;
    Ennemi::camera = camera;
    Ennemi::pvJoueur = pvJoueur;
    Ennemi::shieldJoueur = shieldJoueur ; 
    Ennemi::listeTextures = listeTextures;
    vitesse = 1.3f;
    tempsDerniereAction = GetTime();
    poursuite = false;
    dead = false;
    itemType = -1 ; 
    typeItem = 0 ; 
    checkDrop = 0 ; 
    dropItem = false ; 
    SetRandomType();
    SetRandomPos();

    tagEnnemi ++ ;
    tagEnnemiPerso = tagEnnemi;

}

int Ennemi::nbKill = 0; 

void Ennemi::SetRandomType(void)
{
    const int nombreDeTypes = 4; // A CHANGER QUAND ON RAJOUTE DES ENNEMIS
    float listeTailles[] =  {1.0f,  0.9f,   1.6f,   0.4f}; //
    int listePvs[] =        {100,   100,    200,    3455};      //
    int listeDegats[] =     {5,     20,     30,     -20};

    typeEnnemi = 0;
    taille = listeTailles[typeEnnemi];
    defaultY = taille/4.0f;
    distanceCollision = taille/6.0f;
    pvMax = listePvs[typeEnnemi];
    pv = pvMax;
    degats = listeDegats[typeEnnemi];
}

bool Ennemi::VisionDirecte(Vector3 pos1)
{
    Vector3 pos2 = position;
    bool visionDirecte = true;
    Vector2 croisement;
    float decal = 0.1f;
    for(int y=0; y<dimensionsMap.height; y++)
        for(int x=0; x<dimensionsMap.width; x++)
            if((mapCouleurs[y*dimensionsMap.width + x].r == 255) && (
                CheckCollisionLines((Vector2){pos1.x, pos1.z}, (Vector2){pos2.x, pos2.z}, 
                                    (Vector2){mapPosition.x + x*1.0f - 0.5f - decal, mapPosition.z + y*1.0f - 0.5f - decal},
                                    (Vector2){mapPosition.x + x*1.0f + 0.5f + decal, mapPosition.z + y*1.0f + 0.5f + decal},
                                    &croisement) ||
                CheckCollisionLines((Vector2){pos1.x, pos1.z}, (Vector2){pos2.x, pos2.z}, 
                                    (Vector2){mapPosition.x + x*1.0f + 0.5f + decal, mapPosition.z + y*1.0f - 0.5f - decal},
                                    (Vector2){mapPosition.x + x*1.0f - 0.5f - decal, mapPosition.z + y*1.0f + 0.5f + decal},
                                    &croisement)
                )
            )
                visionDirecte = false;
    return visionDirecte;
}

bool Ennemi::CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center,
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

void Ennemi::CorrigerDeplacementMurs()
{   
    int caseX = (int)(position.x - mapPosition.x + 0.5f);
    int caseY = (int)(position.z - mapPosition.z + 0.5f);
    Vector2 pointCollision;

    if((mapCouleurs[(caseY-1)*dimensionsMap.width+caseX].r == 255
        && CheckCollisionLineCircle((Vector2){mapPosition.x + caseX*1.0f - 0.5f, mapPosition.z + caseY*1.0f - 0.5f},
                            (Vector2){mapPosition.x + caseX*1.0f + 0.5f, mapPosition.z + caseY*1.0f - 0.5f}, 
                            (Vector2){position.x, position.z},
                            distanceCollision, &pointCollision)
        && position.z < anciennePosition.z
        ) ||
        (mapCouleurs[(caseY+1)*dimensionsMap.width+caseX].r == 255
        && CheckCollisionLineCircle((Vector2){mapPosition.x + caseX*1.0f - 0.5f, mapPosition.z + caseY*1.0f + 0.5f},
                            (Vector2){mapPosition.x + caseX*1.0f + 0.5f, mapPosition.z + caseY*1.0f + 0.5f}, 
                            (Vector2){position.x, position.z},
                            distanceCollision, &pointCollision)
        && position.z > anciennePosition.z
        )
    )
    {
        position.z = anciennePosition.z;
    } 
        
    if((mapCouleurs[caseY*dimensionsMap.width+caseX-1].r == 255
        && CheckCollisionLineCircle((Vector2){mapPosition.x + caseX*1.0f - 0.5f, mapPosition.z + caseY*1.0f - 0.5f},
                            (Vector2){mapPosition.x + caseX*1.0f - 0.5f, mapPosition.z + caseY*1.0f + 0.5f}, 
                            (Vector2){position.x, position.z},
                            distanceCollision, &pointCollision)
        && position.x < anciennePosition.x
        ) ||
        (mapCouleurs[caseY*dimensionsMap.width+caseX+1].r == 255
        && CheckCollisionLineCircle((Vector2){mapPosition.x + caseX*1.0f + 0.5f, mapPosition.z + caseY*1.0f - 0.5f},
                            (Vector2){mapPosition.x + caseX*1.0f + 0.5f, mapPosition.z + caseY*1.0f + 0.5f}, 
                            (Vector2){position.x, position.z},
                            distanceCollision, &pointCollision)
        && position.x > anciennePosition.x
        )
    )
    {
        position.x = anciennePosition.x;      
    }  
}

float Ennemi::DistanceHorizontale(Vector3 pos1, Vector3 pos2)
{
    float x1, x2, z1, z2;
    x1 = pos1.x;
    z1 = pos1.z;
    x2 = pos2.x;
    z2 = pos2.z;
    return sqrt(pow(x1 - x2, 2) + pow(z1 - z2, 2));
}

Vector3 Ennemi::GetRandomPos()
{
    int xMax = dimensionsMap.width/2;
    int zMax = dimensionsMap.height/2;
    int caseX, caseZ;
    float posX, posZ;
    do
    {
        caseX = GetRandomValue(0, 2*xMax - 1);
        caseZ = GetRandomValue(0, 2*zMax - 1);
    }
    while (mapCouleurs[caseZ*dimensionsMap.width + caseX].r == 255);
    posX = (float)caseX - (float)xMax - 0.5f + (float)GetRandomValue(0, 99) / 100.0f;
    posZ = (float)caseZ - (float)zMax - 0.5f + (float)GetRandomValue(0, 99) / 100.0f;

    return (Vector3) {posX, defaultY, posZ};
}

void Ennemi::SetRandomPos()
{
    Vector3 cameraPos = (*camera).position;
    do
    {
        position = GetRandomPos();
    }
    while (VisionDirecte(cameraPos)
        || DistanceHorizontale(cameraPos, position) < 3
        );
    anciennePosition = position;
}

void Ennemi::Damaged(int dmg)
{
    if(item.instaKillEffect == true) pv = 1 ;
    pv -= dmg;
    if(pv <= 0){
        posDead = anciennePosition ; 
        dead = true;
        nbKill = nbKill + 1 ;
        }

// Init des items 
    if(dead == true){

        typeItem = GetRandomValue(1,10);

        if(typeItem == 1 || typeItem == 2 || typeItem == 3 || typeItem == 4 || typeItem == 5){
            dropItem = true ;
            item.Init(camera, typeItem);
        }


    }
}
    

    


void Ennemi::Render()
{
    if(!dead)
    {
    DrawCylinder((Vector3){position.x, 0.0f, position.z}, taille/8.0f, taille/8.0f, 0.0001f, 16, DARKGRAY);
    DrawBillboard(*camera, listeTextures[typeEnnemi], position, taille/2.0f, WHITE);
    }
}

void Ennemi::CourtVersJoueur()
{
    Vector3 posJoueur = (*camera).position;
    Vector3 directionUnitaire = (Vector3){posJoueur.x - position.x, 0.0f, posJoueur.z - position.z};

    float norme = sqrt(pow(directionUnitaire.x, 2) + pow(directionUnitaire.z, 2));
    directionUnitaire.x /= norme;
    directionUnitaire.z /= norme;

    double temps = GetTime();
    double tempsPasse = temps - tempsDerniereAction;
    tempsDerniereAction = temps;
    float distanceParcourue = vitesse * tempsPasse;
    position = (Vector3){anciennePosition.x + distanceParcourue * directionUnitaire.x, defaultY,
                         anciennePosition.z + distanceParcourue * directionUnitaire.z};
    CorrigerDeplacementMurs();
    anciennePosition = position;
}

bool Ennemi::CourtVersDestination()
{
    Vector3 directionUnitaire = (Vector3){destination.x - position.x, 0.0f, destination.z - position.z};

    float norme = sqrt(pow(directionUnitaire.x, 2) + pow(directionUnitaire.z, 2));
    directionUnitaire.x /= norme;
    directionUnitaire.z /= norme;

    double temps = GetTime();
    double tempsPasse = temps - tempsDerniereAction;
    tempsDerniereAction = temps;
    float distanceParcourue = vitesse * tempsPasse;
    position = (Vector3){anciennePosition.x + distanceParcourue * directionUnitaire.x, defaultY,
                         anciennePosition.z + distanceParcourue * directionUnitaire.z};
    CorrigerDeplacementMurs();
    anciennePosition = position;

    if(sqrt(pow(position.x - destination.x, 2) + pow(position.z - destination.z, 2)) < 0.5f) return true;
    return false;
}

void Ennemi::Action()
{
    if(!dead)
    {
    Vector3 cameraPos = (*camera).position;
    //if(VisionDirecte(position, (*camera).position)) poursuite = true;
    //std::cout << "d " << destination.x << " " << destination.z << std::endl;

    if(poursuite)
    {
        CourtVersJoueur();
    }
    else
    {
        if(!actionEnCours)  // Nouvelle action
        {
            mode = GetRandomValue(1,2);
            if(mode == 1)    // Deplacement
            {
                do
                {
                    destination = GetRandomPos();
                } while (!VisionDirecte(destination));
                
            }
            else             // Standby
            {
                dureeAttente = GetRandomValue(1,3);
                debutAttente = GetTime();
            }
            actionEnCours = true;
        }

        else
        {
            if(mode==1)
            {
                if (CourtVersDestination())
                    actionEnCours = false;
            }
            else
            {
                if(GetTime() - debutAttente > dureeAttente)
                    actionEnCours = false;
            }
        }
    }

    distJoueur = sqrt(pow(position.x - cameraPos.x, 2) + pow(position.z - cameraPos.z, 2));
    if(GetTime() - derniereAttaque > 1 && distJoueur < distanceCollision)
    {
        if(item.shieldJoueurTest <= 0) *pvJoueur -= degats ;
        else if (item.shieldJoueurTest - degats >= 0) item.shieldJoueurTest -= degats ;
        else if (item.shieldJoueurTest - degats < 0){
            *pvJoueur -= item.shieldJoueurTest ;
            item.shieldJoueurTest = 0 ;
        }
        derniereAttaque = GetTime();
    }
    tempsDerniereAction = GetTime();
    }
}