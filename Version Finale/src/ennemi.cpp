#include "ennemi.h"
#include "raylib.h"
#include "raymath.h"
#include "cmath"
#include "string"
#include "iostream"
#include "string"
#include "numeric"
#include "unordered_map"
#include "vector"
#include "algorithm"

int Ennemi::nbKill = 0; 
int Ennemi::argent = 15000;

bool Ennemi::operator<(Ennemi const &other) const
{
    return this->distJoueur < other.distJoueur;
}

bool Ennemi::operator>(Ennemi const &other) const
{
    return this->distJoueur > other.distJoueur;
}

bool Ennemi::operator==(const Ennemi *other) const
{
    return this->distJoueur == other->distJoueur;
}

void Ennemi::Init(std::vector<Ennemi> *ennemis, Color *mapCouleurs, Texture2D dimensionsMap, Vector3 mapPosition, Camera *camera,
                  int *pvJoueur, Texture2D *listeTextures, int *shieldJoueur, int shieldJoueurMax,
                  int *ennemisRestants, std::unordered_map<std::string, bool> *newEffects, int *ammo,
                  int *clip, bool* unlocked, int *getAmmo)
{
    Ennemi::ennemis = ennemis;
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
    Ennemi::getAmmo = getAmmo;
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
    itemState = false;
    dead = false;
    dmged = false;
    SetRandomType();
    SetRandomPos();
};

void Ennemi::SetRandomType(void)
{
    /*                      0 AMOGUS
                            1 TREE 
                            2 SIRENHEAD 
                            3 CAGASTE
                            4 UGANDAN KNUCKLES 
                            5 BLUE LOBSTER
                            6 OBUNGA
                            7 PURPLE GUY
                            8 BIG CHUNGUS
                            9 TRUC DOOM
                            10 DOG
                            0       1       2       3       4       5       6       7       8       9       10
    */
    float listeTailles[] =  {1.0f,  0.9f,   1.6f,   1.3f,   0.6f,   1.2f,   1.25f,  1.1f,   1.2f,   1.3f,   1.0f};
    int listePvs[] =        {100,   100,    200,    400,    70,     170,    160,    120,    250,    500,    130};
    int listeDegats[] =     {5,     20,     30,     30,     10,     20,     15,     15,     20,     50,     10};
    float listeVitesses[] = {1.7f,  1.2f,   1.4f,   1.6f,   2.2f,   1.5f,   1.8f,   1.6f,   1.3f,   1.4f,   1.5f};
    int listeArgent[] =     {50,    75,     100,    200,    50,     75,     70,     55,     60,     250,    80};

    
    const int nombreDeTypes = sizeof(listePvs)/sizeof(int);
    typeEnnemi = GetRandomValue(0, nombreDeTypes-1);
    taille = listeTailles[typeEnnemi];
    defaultY = taille/4.0f;
    distanceCollision = taille/6.0f;
    pvMax = listePvs[typeEnnemi];
    pv = pvMax;
    degats = listeDegats[typeEnnemi];
    vitesse = listeVitesses[typeEnnemi];
    money = listeArgent[typeEnnemi];
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
    float portions = (float) 9;
    Vector2 pos1, pos2;
    for(float alpha=0; alpha < 2*PI; alpha += 2*PI/portions)
    {
        pos1.x = (float) center.x + radius*cos(alpha),                 pos1.y = (float) center.y + radius*sin(alpha);
        pos2.x = (float) center.x + radius*cos(alpha + 2*PI/portions), pos2.y = (float) center.y + radius*sin(alpha + 2*PI/portions);
        if(CheckCollisionLines(pos1, pos2, startPos, endPos, pointCollision)) return true;
    }
    return false;
}

void Ennemi::CorrigerDeplacementMursEtEnnemis()
{   
    int caseX = (int)(position.x - mapPosition.x + 0.5f);
    int caseY = (int)(position.z - mapPosition.z + 0.5f);
    Vector2 pointCollision;

    for(int i=0; i<ennemis->size(); i++)
    {
        Vector3 posi = ennemis->at(i).position;
        float taillei = ennemis->at(i).taille;
        float ecart = (taillei + taille)/6.0f;
        if(!ennemis->at(i).dead && CheckCollisionCircles(Vector2{position.x, position.z}, taillei/6.0f, Vector2{ennemis->at(i).position.x, ennemis->at(i).position.z}, ennemis->at(i).taille/6.0f))
        {
            if(&ennemis->at(i) != this)
            {
                if(Vector2Distance(Vector2{posi.x, posi.z}, Vector2{position.x, position.z}))
                {
                    position = Vector3Add(posi, Vector3Scale(Vector3Normalize(Vector3Subtract(position, posi)), ecart));
                }
            }
        }
    }
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
    anciennePosition = position;
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
    argent += money ;
    *ennemisRestants -= 1;

    //munitions 30%, instakill 4%, infiniteammo 5%, shield 10%, radar 10
    float probas[] = {30.0f, 4.0f, 5.0f, 10.0f, 10.0f};
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
    else if(rand < std::accumulate(probas, probas + 5, 0.0f))
    {
        itemType = "radar";
        itemState = true;
    }
    if(itemState) itemStateTexture = LoadTexture(("../resources/items/" + itemType + ".png").c_str());
    decal = 0.0f;
}

void Ennemi::Damaged(int dmg)
{
    pv -= dmg;
    dmged = true;
    dmgTime = GetTime();
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
        Color c;
        double x = GetTime() - dmgTime;
        float duree = 0.4f;
        if(x < duree)
        {
            std::cout << x << std::endl;
            c = (Color){255, 255.0*pow(x/duree, 2), 255.0*pow(x/duree, 2), 255};
        }
        else
            c = WHITE;
        DrawBillboard(*camera, listeTextures[typeEnnemi], position, taille/2.0f, c);
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
    CorrigerDeplacementMursEtEnnemis();
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
    CorrigerDeplacementMursEtEnnemis();

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
                int num = std::stoi(itemType.substr(4, 1));
                ammo[num] += getAmmo[num];
                std::string strAmmoSound = ("../resources/sound/ammo") + std::to_string(num) + (".wav");
                ammoSound = LoadSound(strAmmoSound.c_str());
                PlaySound(ammoSound);
                //UnloadSound(ammoSound);
                
            }
            else if(itemType == "shield")
            {
                shieldSound = LoadSound("../resources/sound/itemShield1.wav");
                PlaySound(shieldSound);
                //UnloadSound(shieldSound);
                *shieldJoueur = std::min(*shieldJoueur + 25, shieldJoueurMax);
            }
            else if(itemType == "instakill")
            {
                instaKillSound = LoadSound("../resources/sound/instaKill.wav");
                PlaySound(instaKillSound);
                //UnloadSound(instaKillSound);
                (*newEffects)["instakill"] = true;
                
            }
            else if(itemType == "infiniteammo")
            {
                maxAmmo = LoadSound("../resources/sound/maxAmmo.wav");
                PlaySoundMulti(maxAmmo);
                //UnloadSound(maxAmmo);
                (*newEffects)["infiniteammo"] = true;
            }
            else if(itemType == "radar")
            {
                radar = LoadSound("../resources/sound/radar.wav");
                PlaySoundMulti(radar);
                //UnloadSound(maxAmmo);
                (*newEffects)["radar"] = true;
            }
            std::vector<Ennemi>::iterator it = std::find((*ennemis).begin(), (*ennemis).end(), this);
            (*ennemis).erase(it);
        }
    }
    else if(!dead)
    {
    Vector3 cameraPos = (*camera).position;
    distJoueur = sqrt(pow(position.x - cameraPos.x, 2) + pow(position.z - cameraPos.z, 2));
    if(VisionDirecte(cameraPos) && distJoueur < 6 || dmged) poursuite = true;

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

    if(GetTime() - derniereAttaque > 1 && distJoueur < distanceCollision)
    {
        if(*shieldJoueur > 0) *shieldJoueur = std::max(*shieldJoueur - degats, 0);
        else *pvJoueur = std::max(*pvJoueur - degats, 0);
        derniereAttaque = GetTime();
    }
    tempsDerniereAction = GetTime();
    }
}