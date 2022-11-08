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
#include "item.h"
#include "projectile.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

#define PLEIN_ECRAN 1
#define MAX_ENNEMIS 500
#define MAX_PROJECTILES 50


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

// .......................................................................

int main(int argc, char const *argv[])
{
    const int largeur = (PLEIN_ECRAN == 1)? 1920 : 1080;
    const int hauteur = (PLEIN_ECRAN == 1)? 1080 : 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(largeur, hauteur, "BACKDOOM");
    SetWindowState((PLEIN_ECRAN == 1)?FLAG_FULLSCREEN_MODE : 0);

    // Variables Menus 
    bool reset ;
    int playSound = 0 ;
    int playSoundBoucle = 0 ;

    // On fait la caméra
    Camera3D camera = Camera();
    camera.position = (Vector3){0.0f, 0.4f, 2.0f};
    camera.target = (Vector3){0.0f, 0.4f, 0.5f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.projection = CAMERA_PERSPECTIVE;
    int frameCounter = 0;

    // Shaders  + récupération des informations nécessaires au fonctionnement du shader

    Shader shader = LoadShader(TextFormat("../resources/shaders/glsl100/lighting.vs", GLSL_VERSION),
                                TextFormat("../resources/shaders/glsl100/fog.fs", GLSL_VERSION));

    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");

    int ambientLoc = GetShaderLocation(shader, "ambient"); //luminosité ambiante
    float ambiante[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    SetShaderValue(shader, ambientLoc, ambiante, SHADER_UNIFORM_VEC4);

    float fogDensity = 0.15f;       // épaisseur du brouillard
    int fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    Light lumiere = CreateLight(LIGHT_POINT,{1.0f,0.9f,0.0f},camera.position,YELLOW,shader);


    // Details joueur
    float distanceCollision = 0.2f;
    float vitesse = 1.0f;
    int pvJoueur = 5;
    int pvJoueurMax = 100; 
    int shieldJoueur = 0;
    int shieldJoueurMax = 150; 

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
    modeleMap.materials[0].shader = shader; 
    Texture2D *texturesEnnemis = InitTexturesEnnemis();
    Texture2D *texturesEffetsRoquette = InitTexturesRoquette();

    InitAudioDevice();

    // Init item
    Item itemInitial ;
    itemInitial.Init(&camera, 1);

    // Init ennemis
    Ennemi ennemis[MAX_ENNEMIS];
    for(int n=0; n<MAX_ENNEMIS; n++) ennemis[n].Init(mapCouleurs, dimensionsMap, mapPosition, &camera, &pvJoueur, texturesEnnemis, &shieldJoueur, &shieldJoueurMax);

    // Init projectiles
    Projectile projectiles[MAX_PROJECTILES];
    for(int n=0; n<MAX_PROJECTILES; n++) projectiles[n].Init(ennemis, MAX_ENNEMIS, mapCouleurs, dimensionsMap, mapPosition, projectiles,
                                                             MAX_PROJECTILES, &frameCounter, texturesEffetsRoquette, &camera, largeur, hauteur);

    // Init arme
    Arme arme;
    arme.Init(largeur, hauteur, &frameCounter, ennemis, MAX_ENNEMIS, &camera, projectiles, MAX_PROJECTILES);
    
    // Init ATH
    Ath ath;
    ath.Init(largeur, hauteur, &arme, &pvJoueur, &(ennemis[0].nbKill));

    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    SetTargetFPS(60);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    while(!WindowShouldClose())
    {
        BeginShaderMode(shader);

        // Initialisation shader
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

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

        // Action ennemis
        for(int n=0; n<MAX_ENNEMIS; n++) ennemis[n].Action();

        // Tri ennemis par proximité avec le joueur dans le sens croissant
        std::sort(ennemis, ennemis + MAX_ENNEMIS,
          [](Ennemi const & a, Ennemi const & b) -> bool
          { return a.distJoueur < b.distJoueur; } );

        // Sons des pas

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
            itemInitial.shieldJoueurTest = 0 ;
            ennemis[0].nbKill = 0;
            camera.position = {0.0f, 0.4f, 2.0f};
            for(int n=0; n<MAX_PROJECTILES; n++) ennemis[n].Init(mapCouleurs, dimensionsMap, mapPosition, &camera, &pvJoueur, texturesEnnemis, &shieldJoueur, &shieldJoueurMax);
            reset = false;
        }


        BeginDrawing();

            ClearBackground(MAGENTA);
            
            BeginMode3D(camera);
                // Affichage map
                DrawModel(modeleMap, mapPosition, 1.0f, WHITE);

                // Affichage ennemis du plus loin au plus proche
                for(int n=MAX_ENNEMIS-1; n>=0; n--){ 
                    ennemis[n].Render();

                    if((ennemis[n].dropItem == true)&& ennemis[n].item.recupItem == false){
                        ennemis[n].item.Render(ennemis[n].posDead, ennemis[n].typeItem);
                    }
                    if(ennemis[n].typeItem == 5 && (ennemis[n].item.instaKillEffect == true || ennemis[n].item.recupItem == true)) itemInitial.InstaKill() ; 
                    if(ennemis[n].dropItem == true && ennemis[n].item.recupItem == true){
                         ennemis[n].posDead.x = 1111111.0f ;
                         ennemis[n].item.recupItem = false;
                         ennemis[n].item.Render(ennemis[n].posDead, ennemis[n].typeItem);
                    }
                }

                // Affichage projectiles
                for(int n=0; n<MAX_PROJECTILES; n++) projectiles[n].Render();

                // Affichage écran de game over

            EndMode3D();
            EndShaderMode();

            arme.Render();
            if(reset == false && pvJoueur > 0) ath.Render(); 

            if(pvJoueur <= 0){
                ath.gameOverRender();
            }
            fogDensity = 0.00f; 
            SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);
            DrawFPS(10,10);

        EndDrawing();

    }

    UnloadImageColors(mapCouleurs);
    UnloadTexture(dimensionsMap);
    UnloadTexture(textureMap);
    UnloadModel(modeleMap);

    UnloadSound(itemInitial.soundItem);
    UnloadTexture(itemInitial.textureItem);
    UnloadShader(shader);   

    CloseAudioDevice();

    CloseWindow();  
    return 0;
}
