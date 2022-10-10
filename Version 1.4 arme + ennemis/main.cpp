#include "raylib.h"
#include "raymath.h"
#include "iostream"
#include "cmath"
#include "rcamera.h"
#include "algorithm"
#include "string"
#include "ennemi.h"
#include "arme.h"

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

void Deplacement(Camera *camera, float *vitesse, Vector3 anciennePosition)
{
    // Anti vectoring
    if (IsKeyDown(KEY_W) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) *vitesse /= (float) sqrt(2.0);

    // Check sprint
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))
        {
            *vitesse = 1.6f;
            (*camera).fovy = 60.0f;
        }
        else
        {
            *vitesse = 1.0f;
            (*camera).fovy = 45.0f;
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



#define PLEIN_ECRAN 0
#define NB_ENNEMIS 40

int main(int argc, char const *argv[])
{
    const int largeur = (PLEIN_ECRAN == 1)? 1920 : 1080;
    const int hauteur = (PLEIN_ECRAN == 1)? 1080 : 720;

    InitWindow(largeur, hauteur, "BACKDOOM");
    SetWindowState((PLEIN_ECRAN == 1)?FLAG_FULLSCREEN_MODE : 0);

    // On fait la caméra
    Camera3D camera = Camera();
    camera.position = (Vector3){0.0f, 0.5f, 2.0f};
    camera.target = (Vector3){0.0f, 0.5f, 0.5f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Details joueur
    float distanceCollision = 0.2f;
    float vitesse = 1.0f;
    int pvJoueur = 100;

    // Generation map
    Image imageMap = LoadImage("../resources/map/map.png");
    Texture2D dimensionsMap = LoadTextureFromImage(imageMap);
    Mesh mesh = GenMeshCubicmap(imageMap, (Vector3){ 1.0f, 1.0f, 1.0f });
    Model modeleMap = LoadModelFromMesh(mesh);

    Color *mapCouleurs = LoadImageColors(imageMap);
    UnloadImage(imageMap);

    Vector3 mapPosition = {-0.5f * dimensionsMap.width, 0.0f, -0.5f * dimensionsMap.height};
    Vector3 anciennePosition;

    // Textures
    Texture2D textureMap = LoadTexture("../resources/map/mapAtlas.png");
    modeleMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textureMap;
    Texture2D *texturesEnnemis = InitTexturesEnnemis();

    // Init ennemis
    int m = NB_ENNEMIS;
    Ennemi ennemis[m];
    for(int n=0; n<m; n++) ennemis[n].Init(mapCouleurs, dimensionsMap, mapPosition, &camera, &pvJoueur, texturesEnnemis);

    // Init arme
    int frameCounter = 0;
    Arme arme;
    arme.Init(largeur, hauteur, &frameCounter, ennemis, NB_ENNEMIS, &camera);

    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        anciennePosition = camera.position;
        UpdateCamera(&camera);
        frameCounter++;
        
        // Action arme
        if(IsKeyPressed(KEY_R))
            arme.Recharger();
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            arme.Tirer();

        // Action ennemis
        for(int n=0; n<m; n++) ennemis[n].Action();

        // Tri ennemis par proximité avec le joueur dans le sens croissant
        std::sort(ennemis, ennemis + m,
          [](Ennemi const & a, Ennemi const & b) -> bool
          { return a.distJoueur < b.distJoueur; } );

        // Déplacement caméra
        Deplacement(&camera, &vitesse, anciennePosition);

        // Check collisions
        camera.position = CorrigerDeplacementMurs(camera.position, anciennePosition, distanceCollision,
                                                  mapPosition, mapCouleurs, dimensionsMap);

        BeginDrawing();

            ClearBackground(MAGENTA);
            
            BeginMode3D(camera);
                // Affichage map
                DrawModel(modeleMap, mapPosition, 1.0f, WHITE);

                // Affichage ennemis du plus loin au plus proche
                for(int n=m-1; n>=0; n--) ennemis[n].Render();
            EndMode3D();
            arme.Render();

            DrawFPS(10,10);
            std::string strPv = "PV : " + std::to_string(pvJoueur);
            DrawText(strPv.c_str(), 10, 30, 30, GREEN);

        EndDrawing();


    }

    UnloadImageColors(mapCouleurs);
    UnloadTexture(dimensionsMap);
    UnloadTexture(textureMap);
    UnloadModel(modeleMap);

    CloseWindow();  
    return 0;
}
