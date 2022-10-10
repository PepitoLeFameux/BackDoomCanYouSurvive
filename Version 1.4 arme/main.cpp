#include "raylib.h"
#include "iostream"
#include "cmath"
#include "rcamera.h"


/*  TODO en priorité : Tirer avec une arme (affichage sprite de l'arme, curseur, munitions, rechargement, cadence de tir, dégats)
                      Ennemis se dirigent aléatoirement et tirent vers le joueur (attaque à distance) 
                      Mettre PV pour joueur/ennemis + écran de mort 
                      dégâts provoqués par ennemis
                      Réparer déplacement latéral caméra
                      Changer texture du plafond
                      changer map
                      ATH : arme, PV, munitions, score, (minimap ?) + devient rouge lorsque des dégâts sont pris
                      timer
                      Faire spawn plusieurs ennemis (stockage dans un array ?)
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

bool CheckVision(Vector3 pos1, Vector3 pos2, Texture2D dimensionsMap, Vector3 mapPosition, Color *mapCouleurs)
{
    bool visionDirecte = true;
    Vector2 croisement;
    for(int y=0; y<dimensionsMap.height; y++)
        for(int x=0; x<dimensionsMap.width; x++)
        if((mapCouleurs[y*dimensionsMap.width + x].r == 255) && (
            CheckCollisionLines((Vector2){pos1.x, pos1.z}, (Vector2){pos2.x, pos2.z}, 
                                (Vector2){mapPosition.x + x*1.0f - 0.5f, mapPosition.z + y*1.0f - 0.5f},
                                (Vector2){mapPosition.x + x*1.0f + 0.5f, mapPosition.z + y*1.0f + 0.5f},
                                &croisement) ||
            CheckCollisionLines((Vector2){pos1.x, pos1.z}, (Vector2){pos2.x, pos2.z}, 
                                (Vector2){mapPosition.x + x*1.0f + 0.5f, mapPosition.z + y*1.0f - 0.5f},
                                (Vector2){mapPosition.x + x*1.0f - 0.5f, mapPosition.z + y*1.0f + 0.5f},
                                &croisement)
        ))
        {
            visionDirecte = false;
        }
    return visionDirecte;
}


int main(int argc, char const *argv[])
{
    const int largeur = 1080;
    const int hauteur = 720;

    InitWindow(largeur, hauteur, "BACKDOOM");
    //SetWindowState(FLAG_FULLSCREEN_MODE);

    //variables des armes
    int nbTir = 0;
    int ammo = 13;
    double time = 0;
    double previousTime = 0;
    bool verifTir = false ; //variable pour savoir si le joueur peut tirer (cadence)

    //variables des animations
    int debutAnim = 0 ;
    int frameCounter = 0;
    int frameSpeed = 20;
    int currentFrame = 1;

    // On fait la caméra
    Camera3D camera = Camera();
    camera.position = (Vector3){0.0f, 0.5f, 2.0f};
    camera.target = (Vector3){0.0f, 0.5f, 1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Generation map
    Image imageMap = LoadImage("../resources/map.png");
    Texture2D dimensionsMap = LoadTextureFromImage(imageMap);
    Mesh mesh = GenMeshCubicmap(imageMap, (Vector3){ 1.0f, 1.0f, 1.0f });
    Model modeleMap = LoadModelFromMesh(mesh);

    Color *mapCouleurs = LoadImageColors(imageMap);
    UnloadImage(imageMap);

    Vector3 mapPosition = {-0.5f * dimensionsMap.width, 0.0f, -0.5f * dimensionsMap.height};

    // Textures
    Texture2D texture = LoadTexture("../resources/mapAtlas.png");
    modeleMap.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Texture2D amogus = LoadTexture("../resources/amogus.png");
    Vector3 posAmogus = (Vector3){0.0f, 0.25f, 1.0f};

    Texture2D weapon = LoadTexture("../resources/weapons/weapon1.png");
    Texture2D reticule = LoadTexture("../resources/weapons/reticule.png");
    const char * name = "";

    // Details joueur
    float distanceCollision = 0.02f;
    float vitesse = 1.0f;


    SetCameraMode(camera, CAMERA_FIRST_PERSON);
    SetTargetFPS(60);


    while(!WindowShouldClose())
    {
        // Calcul du temps passé
        Vector3 anciennePosition = camera.position;
        UpdateCamera(&camera);
        frameCounter ++ ;
        //timeCounter ++ ;

        // Sprint
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyDown(KEY_W) && !IsKeyDown(KEY_S))
        {
            vitesse = 1.6f;
            camera.fovy = 60.0f;
        }
        else
        {
            vitesse = 1.0f;
            camera.fovy = 45.0f;
        }

        // Anti vectoring
        if (IsKeyDown(KEY_W) && (IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) vitesse /= (float) sqrt(2.0);

        // Vitesse
        Vector3 ecartPosition = (Vector3){camera.position.x - anciennePosition.x,
                                          camera.position.y - anciennePosition.y,
                                          camera.position.z - anciennePosition.z};
        

        Vector2 posJoueur = {camera.position.x, camera.position.z};

        camera.position = (Vector3){vitesse*ecartPosition.x + anciennePosition.x,
                                    vitesse*ecartPosition.y + anciennePosition.y,
                                    vitesse*ecartPosition.z + anciennePosition.z};

        // Coordonnées de la case du joueur
        int caseJoueurX = (int)(posJoueur.x - mapPosition.x + 0.5f);
        int caseJoueurY = (int)(posJoueur.y - mapPosition.z + 0.5f);
        //std::cout << caseJoueurX << " " << caseJoueurY << std::endl;

        // Check collisions
        for(int y=0; y<dimensionsMap.height; y++)
        for(int x=0; x<dimensionsMap.width; x++)
        if((mapCouleurs[y*dimensionsMap.width + x].r == 255) &&
            (CheckCollisionCircleRec(posJoueur, distanceCollision,
            (Rectangle){ mapPosition.x + x*1.0f - 0.5f, mapPosition.z + y*1.0f - 0.5f, 1.0f, 1.0f })))
            {
                camera.position = anciennePosition;
            }

        // Check vision amogus...................................................................................
        Vector2 croisement = (Vector2){0.0f, 0.0f};
        bool vueDirecte = CheckVision(camera.position, posAmogus, dimensionsMap, mapPosition, mapCouleurs);
        
        std::cout << vueDirecte << std::endl;

        //Gestion des armes........................................................................................
        Vector3 posWeapon = {camera.target.x,camera.target.y,camera.target.z};

        if (IsKeyPressed(KEY_R) == true) {
            ammo = 13 ;}                     //rechargement

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) == true) verifTir = true; //gestion de la cadence de tir

        if (verifTir == true){
            if (nbTir == 0){
                previousTime = GetTime();
                ammo --;
                debutAnim = 1;
            }
            if (nbTir == 1){
                time = GetTime();
        
                if ((time - previousTime) < 0.3){}

                if ((time - previousTime) >= 0.3){
                    if (ammo != 0){
                    previousTime = time;
                    ammo --;
                    debutAnim = 1;
                }
            }
        } 
        verifTir = false;
        nbTir = 1;
     }
    

        BeginDrawing();

            ClearBackground(MAGENTA);

            BeginMode3D(camera);
                DrawModel(modeleMap, mapPosition, 1.0f, WHITE);
                DrawSphere((Vector3){0.0f - 0.5f, 0.0f, -1.0f - 0.5f}, 0.3f, RED);
                //DrawBillboard(camera,reticule,posAmogus,0.25f,WHITE);
                 DrawBillboard(camera,reticule,posWeapon,0.1645f,WHITE);

//ANIMATION DU TIR DE L'ARME.......................................................................................
                DrawBillboard(camera,weapon,posWeapon,0.1640f,WHITE);  
                if ((debutAnim == 1 || currentFrame != 1)) {
                    
                    if (frameCounter >= (60/frameSpeed)){ //Vitesse d'amination

                        frameCounter = 0;
                        currentFrame ++;
                        
                        if(currentFrame == 2){
                            weapon = LoadTexture("../resources/weapons/weapon2.png");}
                        if(currentFrame == 3){ 
                            weapon = LoadTexture("../resources/weapons/weapon3.png");}
                        if(currentFrame == 4){   
                            weapon = LoadTexture("../resources/weapons/weapon4.png");}
                        if (currentFrame == 5){
                            weapon = LoadTexture("../resources/weapons/weapon1.png");
                            currentFrame = 1;}  

                    debutAnim = 0;
                    }}
//...............................................................................................
              
            EndMode3D();

        EndDrawing();

    }

    UnloadImageColors(mapCouleurs);
    UnloadTexture(dimensionsMap);
    UnloadTexture(texture);
    UnloadTexture(reticule);
    UnloadTexture(weapon);
    UnloadModel(modeleMap);

    CloseWindow();  
    return 0;
}
