#include "ennemi.h"
#include "raylib.h"
#include "raymath.h"
#include "cmath"
#include "string"
#include "iostream"
#include "string"
#include "numeric"
#include "unordered_map"

int Ennemi::nbKill = 0; 

bool Ennemi::operator<(Ennemi const &other) const
{
    return this->distJoueur < other.distJoueur;
}

void Ennemi::Init(Color *mapCouleurs, Texture2D dimensionsMap, Vector3 mapPosition, Camera *camera,
                  int *pvJoueur, Texture2D *listeTextures, int *shieldJoueur, int shieldJoueurMax,
                  int *ennemisRestants, std::unordered_map<std::string, bool> *newEffects, int *ammo,
                  int *clip, bool* unlocked)
{
    Ennemi::mapCouleurs = mapCouleurs;
    Ennemi::dimensionsMap = dimensionsMap;
    Ennemi::mapPosition = mapPosition;
    Ennemi::camera = camera;
    Ennemi::pvJoueur = pvJoueur;
    Ennemi::shieldJoueur = shieldJoueur ; 
    Ennemi::listeTextures = listeTextures;
    Ennemi::ennemisRestants = ennemisRestants;
    Ennemi::newEffects = newEffects;
    Ennemi::ammo = ammo;
    Ennemi::clip = clip;
    Ennemi::unlocked = unlocked;
    dead = true;
    position = deadpos;
}

void Ennemi::Spawn()
{
    tempsDerniereAction = GetTime();
    actionEnCours = false;
    poursuite = false;
    dead = false;
    SetRandomType();
    SetRandomPos();
};

void Ennemi::SetRandomType(void)
{
    //                      1 AMOGUS 2 TREE 3 SIRENHEAD
    float listeTailles[] =  {1.0f,  0.9f,   1.6f};
    int listePvs[] =        {100,   100,    200};
    int listeDegats[] =     {5,     20,     30};
    float listeVitesses[] = {1.5f,  1.0f,   1.2f};

    const int nombreDeTypes = sizeof(listePvs)/sizeof(int);
    typeEnnemi = 0;
    taille = listeTailles[typeEnnemi];
    defaultY = taille/4.0f;
    distanceCollision = taille/6.0f;
    pvMax = listePvs[typeEnnemi];
    pv = pvMax;
    degats = listeDegats[typeEnnemi];
    vitesse = listeVitesses[typeEnnemi];
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

void Ennemi::DeathSequence()
{
    dead = true;
    deathTime = GetTime();
    nbKill += 1;
    *ennemisRestants -= 1;

    //munitions 30%, instakill 4%, infiniteammo 5%, shield 10%
    float probas[] = {30.0f, 100.0f, 5.0f, 10.0f};
    float rand = (float)GetRandomValue(0,10000)/100.0f;
    if(rand < std::accumulate(probas, probas + 1, 0.0f))
    {
        unsigned __int8 num, cnt;
        do
        {
            num = GetRandomValue(1,8);
            cnt ++;
        } while (!unlocked[num] && cnt < 50);
        if(cnt < 50)
        {
        itemType = "ammo" + std::to_string(num);
        itemState = true;
        }
    }
    else if(rand < std::accumulate(probas, probas + 2, 0.0f))
    {
        itemType = "instakill";
        itemState = true;
    }
    else if(rand < std::accumulate(probas, probas + 3, 0.0f))
    {
        itemType = "infiniteammo";
        itemState = true;
    }
    else if(rand < std::accumulate(probas, probas + 4, 0.0f))
    {
        itemType = "shield";
        itemState = true;
    }
    if(itemState) itemStateTexture = LoadTexture(("../resources/items/" + itemType + ".png").c_str());
    decal = 0.0f;
}

void Ennemi::Damaged(int dmg)
{
    pv -= dmg;
    if(pv <= 0) DeathSequence();
}
    
void Ennemi::Render()
{
    if(itemState)
    {
        decal = 0.1f*sin(PI*(GetTime() - deathTime));
        DrawBillboard(*camera, itemStateTexture, Vector3Add(position, (Vector3){0.0f, decal, 0.0f}), 0.2f, WHITE);
        DrawCylinder((Vector3){position.x, 0.0f, position.z}, 0.2f, 0.2f, 0.0001f, 16, DARKGRAY);
    }
    else if(!dead)
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
    if(itemState)
    {
        Vector3 cameraPos = (*camera).position;
        distJoueur = sqrt(pow(position.x - cameraPos.x, 2) + pow(position.z - cameraPos.z, 2));
        if(distJoueur < 0.6f)
        {
            itemState = false;
            if(itemType.substr(0, 4) == "ammo")
            {
                __int8 num = std::stoi(itemType.substr(4, 1));
                ammo[num] += clip[num];
            }
            else if(itemType == "shield")
            {
                *shieldJoueur = std::max(*shieldJoueur + 10, shieldJoueurMax);
            }
            else if(itemType == "instakill")
            {
                (*newEffects)["instakill"] = true;
            }
            else if(itemType == "infiniteammo")
            {
                (*newEffects)["infiniteammo"] = true;
            }
        }
    }
    else if(!dead)
    {
    Vector3 cameraPos = (*camera).position;
    if(VisionDirecte(position)) poursuite = true;

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
        if(*shieldJoueur > 0) *shieldJoueur = std::max(*shieldJoueur - degats, 0);
        else *pvJoueur = std::max(*pvJoueur - degats, 0);
        derniereAttaque = GetTime();
    }
    tempsDerniereAction = GetTime();
    }
}