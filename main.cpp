#include <raylib.h>
#include <rcamera.h>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "BackDoom");

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 4.0f, 2.0f, 4.0f };
    camera.target = (Vector3){ 0.0f, 1.8f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

//--------------------Generation de la map---------------------------------------

    // Generates some random columns

    SetCameraMode(camera, CAMERA_ORBITAL); // Set a first person camera mode

    SetTargetFPS(60); 
    //--------------------------------------------------------------------------------------

    Texture2D bill = LoadTexture("../resources/doomsprite.png"); 
    Vector3 billPosition = {0.0f,2.0f,0.0f};                       

    // Main game loop
    while (!WindowShouldClose())                // Detect window close button or ESC key
    {

        UpdateCamera(&camera);

        Vector3 dernierePosCam;        //Check Joueur Collision (2D Collision)
        Vector2 joueurPos = {camera.position.x, camera.position.z};
        
        Rectangle test = {3.0f,0.3f,5.0f,2.5f};

        if (CheckCollisionCircleRec(joueurPos,1.0f,test) == false){
        dernierePosCam = camera.position ;}
        if (CheckCollisionCircleRec(joueurPos,1.0f,test) == true){
            camera.position = dernierePosCam;}

        


        //----------------------------------------------------------------------------------
        UpdateCamera(&camera);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                //Modélisation de la map carrée
                DrawRectangleRec(test,BLACK);

                DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Sol
                DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 0.05f, 3.0f, 32.0f, BLUE);  
                DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 0.05f, 3.0f, 32.0f, LIME);      
                //DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 0.05f, 3.0f, 32.0f, GOLD);
                DrawBillboard(camera, bill, billPosition, 2.0f, WHITE);

                DrawCylinder((Vector3){camera.position.x,0.0f,camera.position.z}, 0.3f, 0.3f, 0.1f, 360, BLACK); //modélisation de la hitbox du joueur 
                // Draw some cubes around

            EndMode3D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}