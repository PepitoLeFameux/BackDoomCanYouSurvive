#include "raylib.h"
#include "raymath.h"
#include "iostream"
#include "cmath"
#include "rcamera.h"
#include "algorithm"
#include "string"
#include "ennemi.h"
#include "arme.h"
#include "ath.h"
#include "projectile.h"
#include "unordered_map"
#include "vector"

#define PLEIN_ECRAN 1
#define MAX_PROJECTILES 100

/*  TODO en priorité : Tirer avec une arme (affichage sprite de l'arme, curseur, munitions, rechargement, cadence de tir, dégats)
                      Ennemis se dirigent aléatoirement et tirent vers le joueur (attaque à distance) 
                      Mettre PV pour joueur/ennemis + écran de mort 
                      X dégâts provoqués par ennemis
                      Réparer déplacement latéral caméra
                      Changer texture du plafond
                      changer map
                      ATH : arme, PV, munitions, score, (minimap ?) + devient rouge lorsque des dégâts sont pris
                      timer
                      X Faire spawn plusieurs ennemis (stockage dans un array ?)
                      Sons ambiance : marche, tirs, ennemis.
                      gestion du score
                      compteur fps
                      trouver sprite animé (3-4 frames) pour ennemi (mort + déplacement ?)

    TODO optionnel : Shaders (ombre et néon) flash sur l'arme ?
                     Changement d'armes
                     spawn munition et (vie ?)(armure ?)
                     Menu
                     Round
                     SafeZone, porte, escalier...
                     Arme CàC
*/

bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center, float radius, Vector2 *pointCollision)
{
    float portions = (float) 8;
    Vector2 pos1, pos2;
    for(float alpha=0; alpha < 2*PI*(1.0f - 1.0f/portions); alpha += 2*PI/8.0f)
    {
        pos1.x = (float) center.x + radius*cos(alpha),             pos1.y = (float) center.y + radius*sin(alpha);
        pos2.x = (float) center.x + radius*cos(alpha + 2*PI/8.0f), pos2.y = (float) center.y + radius*sin(alpha + 2*PI/8.0f);
        if(CheckCollisionLines(pos1, pos2, startPos, endPos, pointCollision)) return true;
    }
    return false;
}

Vector3 CorrigerDeplacementMurs(Vector3 position, Vector3 anciennePosition, float distanceCollision, 
                             Vector3 mapPosition, Color *mapCouleurs, Texture2D dimensionsMap)
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
    return position;
}



Sound footsteps = LoadSound("../resources/sound/footsteps.wav");
Sound running = LoadSound("../resources/sound/running.wav");

void Deplacement(Camera *camera, float *vitesse, Vector3 anciennePosition)
{
    // Anti vectoring
    if (IsKeyDown(KEY_W) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) *vitesse /= (float) sqrt(2.0);

    // Check sprint
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))
        {
            *vitesse = 1.6f;
            (*camera).fovy = 60.0f;
            StopSound(footsteps);
            PlaySound(running);
        }
        else
        {
            *vitesse = 1.0f;
            (*camera).fovy = 45.0f;
            StopSound(running);
            PlaySound(footsteps);
        }
    
    //Déplacement
    (*camera).position = Vector3Lerp(anciennePosition, (*camera).position, *vitesse);
}

Texture2D *InitTexturesEnnemis()
{
    int n=0;
    static Texture2D listeTextures[30];
    while (FileExists(((std::string)("../resources/ennemis/" + std::to_string(n) + ".png")).c_str()))
    {
        listeTextures[n] = LoadTexture(((std::string)("../resources/ennemis/" + std::to_string(n) + ".png")).c_str());
        n++;
    }
    return listeTextures;
}

Texture2D *InitTexturesRoquette()
{
    int n=0;
    static Texture2D listeTextures[30];
    while (FileExists(((std::string)("../resources/projectiles/rpg-" + std::to_string(n) + ".png")).c_str()))
    {
        listeTextures[n] = LoadTexture(((std::string)("../resources/projectiles/rpg-" + std::to_string(n) + ".png")).c_str());
        n++;
    }
    return listeTextures;
}

void SpawnNewEnemy(std::vector<Ennemi> *enemyList, int *remaining, Color *mapCouleurs, Texture2D dimensionsMap, Vector3 mapPosition, Camera *camera,
                  int *pvJoueur, Texture2D *texturesEnnemis, int *shieldJoueur, int shieldJoueurMax,
                  int *ennemisRestants, std::unordered_map<std::string, bool> *newEffects, int *ammo,
                  int *clip, bool* unlocked)
{
    Ennemi newEnemy;
    (*enemyList).push_back(newEnemy);
    (*enemyList).back().Init(enemyList, mapCouleurs, dimensionsMap, mapPosition, camera, pvJoueur, texturesEnnemis,
                  shieldJoueur, shieldJoueurMax, ennemisRestants, newEffects, ammo, clip, unlocked);
    (*enemyList).back().Spawn();
    *remaining += 1;
}

int main(int argc, char const *argv[])
{
    const int largeur = (PLEIN_ECRAN == 1)? 1920 : 1080;
    const int hauteur = (PLEIN_ECRAN == 1)? 1080 : 720;

    InitWindow(largeur, hauteur, "BACKDOOM");
    SetWindowState((PLEIN_ECRAN == 1)?FLAG_FULLSCREEN_MODE : 0);

    // On fait la caméra
    Camera3D camera = Camera();
    camera.position = (Vector3){0.0f, 0.4f, 2.0f};
    camera.target = (Vector3){0.0f, 0.4f, 0.5f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.projection = CAMERA_PERSPECTIVE;
    int frameCounter = 0;

    // Details joueur
    float distanceCollision = 0.2f;
    float vitesse = 1.0f;
    int pvJoueur = 100;
    int pvJoueurMax = 100; 
    int shieldJoueur = 0;
    int shieldJoueurMax = 150; 

    // Jeu
    unsigned __int8 numManche = 1;
    unsigned __int16 spawned;
    bool mancheEnCours;
    bool mancheFinie;
    double debutManche;
    double finManche;
    unsigned __int8 ennemisParManche = 3;
    float secondesParEnnemi;
    double lastSpawn;
    bool reset ; 
    int playSound = 0 ; 

    // Generation map
    Image imageMap = LoadImage("../resources/map/map2.png");
    Texture2D dimensionsMap = LoadTextureFromImage(imageMap);
    Mesh mesh = GenMeshCubicmap(imageMap, (Vector3){ 1.0f, 1.0f, 1.0f });
    Model modeleMap = LoadModelFromMesh(mesh);

    Color *mapCouleurs = LoadImageColors(imageMap);
    UnloadImage(imageMap);

    Vector3 mapPosition = {-0.5f * dimensionsMap.width, 0.0f, -0.5f * dimensionsMap.height};
    Vector3 anciennePosition;

    // Textures et son
    Texture2D textureMap = LoadTexture("../resources/map/mapAtlas.png");
    modeleMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textureMap;
    Texture2D *texturesEnnemis = InitTexturesEnnemis();
    Texture2D *texturesEffetsRoquette = InitTexturesRoquette();

    InitAudioDevice();

    // Init effets
    std::unordered_map<std::string, bool> newEffects = {{"instakill", false}, {"infiniteammo", false}};
    std::unordered_map<std::string, bool> effects = {{"instakill", false}, {"infiniteammo", false}};
    std::unordered_map<std::string, double> effectsEnd = {{"instakill", 0}, {"infiniteammo", 0}};

    // Init valeurs chargeurs
    int ammo[] = {0, 999, 999, 999, 999, 999, 999, 999, 999};
    int clip[] = {0, 0, 13, 5, 2, 100, 1, 666, 666};
    int clipAmmo[] = {0, -1, 13, 5, 2, -1, 1, 666, 666};
    bool unlocked[] = {true, true, true, true, true, true, true, false, false};

    // Init ennemis
    int ennemisRestants = 0;
    std::vector<Ennemi> ennemis;

    // Init projectiles
    Projectile projectiles[MAX_PROJECTILES];
    for(int n=0; n<MAX_PROJECTILES; n++) projectiles[n].Init(&ennemis, mapCouleurs, dimensionsMap, mapPosition, projectiles,
                                                             MAX_PROJECTILES, &frameCounter, texturesEffetsRoquette, &camera, largeur, hauteur);

    // Init arme
    Arme arme;
    arme.Init(largeur, hauteur, &frameCounter, &ennemis, &camera,
              projectiles, MAX_PROJECTILES, &effects, ammo, clip, clipAmmo, unlocked);
    
    // Init ATH
    Ath ath;
    ath.Init(largeur, hauteur, &arme, &pvJoueur, &shieldJoueur, &(ennemis[0].nbKill), &effects);

    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    SetTargetFPS(60);
    

    while(!WindowShouldClose())
    {
        // Action arme
        arme.Action();

        // Action projectiles
        for(int n=0; n<MAX_PROJECTILES; n++) projectiles[n].Action();

        // Caméra
        anciennePosition = camera.position;
        UpdateCamera(&camera);
        frameCounter++;

        // Déplacement caméra
        Deplacement(&camera, &vitesse, anciennePosition);
        camera.position = CorrigerDeplacementMurs(camera.position, anciennePosition, distanceCollision,
                                                  mapPosition, mapCouleurs, dimensionsMap);

        if(IsKeyPressed(KEY_U)){effects["instakill"] = !effects["instakill"];}

        // Action effets
        for(auto const &[effet, etat] : newEffects)
        {
            if(etat)
            {
                newEffects[effet] = false;
                effects[effet] = true;
                effectsEnd[effet] = GetTime() + 10 - numManche/1.5;
            }
            if(effectsEnd[effet] < GetTime())
            {
                effects[effet] = false;
            }
        }

        // Gestion manches
        secondesParEnnemi = std::max((1.0000000 - 0.4*numManche), 0.3);
        if(mancheEnCours)
        {
            if(GetTime() - lastSpawn > secondesParEnnemi)
            {
                SpawnNewEnemy(&ennemis, &ennemisRestants, mapCouleurs, dimensionsMap, mapPosition, &camera, &pvJoueur, texturesEnnemis,
                              &shieldJoueur, shieldJoueurMax, &ennemisRestants, &newEffects, ammo, clip, unlocked);
                spawned++;
                lastSpawn = GetTime();
                if(spawned == int(ennemisParManche * std::pow(1.3, numManche)))
                {
                    mancheEnCours = false;
                    spawned = 0;
                }
            }
        }
        else if(ennemisRestants == 0)
        {
            if(!mancheFinie)
            {
                mancheFinie = true;
                finManche = 10.0 + GetTime();
            }
            else if(GetTime() >= finManche)
            {
                mancheEnCours = true; 
                mancheFinie = false;
                numManche ++;
            }
        }

        // Action ennemis
        for(int n=0; n<ennemis.size(); n++) ennemis[n].Action();
        

        // Tri ennemis par proximité avec le joueur dans le sens croissant
        std::sort(ennemis.begin(), ennemis.end(), std::greater<Ennemi>());

        // Game Over
        if(pvJoueur <= 0){
            if(playSound == 0){
            Sound gameOverSound = LoadSound("../resources/sound/gameOver.wav");
            PlaySound(gameOverSound);
            playSound = 1 ; 
            }
            if(IsKeyDown(KEY_W) || IsKeyDown(KEY_A) || IsKeyDown(KEY_S) || IsKeyDown(KEY_D))camera.position = anciennePosition ;
            if (IsKeyPressed(KEY_ENTER)) reset = true ;
        }

        if (reset == true){
            pvJoueur = 100 ;
            shieldJoueur = 0 ;
            ennemis[0].nbKill = 0;
            camera.position = {0.0f, 0.4f, 2.0f};
            reset = false;
        }

        BeginDrawing();

            ClearBackground(MAGENTA);
            
            BeginMode3D(camera);
                // Affichage map
                DrawModel(modeleMap, mapPosition, 1.0f, WHITE);

                // Affichage ennemis du plus loin au plus proche
                for(int n=0; n<ennemis.size(); n++){ 
                    ennemis[n].Render();
                }

                // Affichage projectiles
                for(int n=0; n<MAX_PROJECTILES; n++) projectiles[n].Render();
            EndMode3D();

            arme.Render();

            if(reset == false && pvJoueur > 0) ath.Render(); 
            if(pvJoueur <= 0){
                ath.gameOverRender();
            }


            if(mancheFinie)
            {
                std::string tempsRestant = std::to_string(int(finManche - GetTime()));
                DrawText(tempsRestant.c_str(), 930, 200, 60, WHITE);
            }

            DrawFPS(10,10);

        EndDrawing();

    }

    UnloadImageColors(mapCouleurs);
    UnloadTexture(dimensionsMap);
    UnloadTexture(textureMap);
    UnloadModel(modeleMap);

    CloseWindow();  
    return 0;
}
