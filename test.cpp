/*******************************************************************************************
*
*   raylib [core] example - 3d camera first person
*
*   Example originally created with raylib 1.3, last time updated with raylib 1.3
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2015-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#define MAX_COLUMNS 20

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.target = (Vector3){ 0.0f, 5.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

//--------------------Generation de la map---------------------------------------

    // Generates some random columns
    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (int)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (int)GetRandomValue(-15, 15), heights[i]/2.0f, (int)GetRandomValue(-15, 15) };
        colors[i] = (Color){ (int)GetRandomValue(20, 255), (int)GetRandomValue(10, 55), 30, 255 };
    }

    SetCameraMode(camera, CAMERA_FIRST_PERSON); // Set a first person camera mode

    SetTargetFPS(60);                           // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())                // Detect window close button or ESC key
    {

       UpdateCamera(&camera);
        Vector3 dernierePosCam;        //Check Joueur Collision (2D Collision)
        Vector2 joueurPos = {camera.position.x, camera.position.z};
        
        Rectangle test = {3.0f,0.3f,5.0f,2.5f};
        bool collision = CheckCollisionCircleRec(joueurPos,1.0f,test);

        if (collision == false){
        dernierePosCam = camera.position ;}
        if (collision == true){
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

                DrawCylinder((Vector3){camera.position.x,0.0f,camera.position.z}, 0.3f, 0.3f, 0.1f, 360, BLACK); //modélisation de la hitbox du joueur 
                // Draw some cubes around
                for (int i = 0; i < MAX_COLUMNS; i++)
                {
                    DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
                    DrawCubeWires(positions[i], 2.0f, heights[i], 2.0f, MAROON);
                }

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