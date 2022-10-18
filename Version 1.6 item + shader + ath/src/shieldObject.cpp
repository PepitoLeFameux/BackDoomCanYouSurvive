#include "shieldObject.h"
#include "raylib.h"
#include "cmath"
#include "ath.h"
#include <cstdio>
#include "arme.h"

int ShieldObject::shieldJoueurTest = 0;
int ShieldObject::shieldJoueurMaxTest = 150;

void ShieldObject::Init(Camera *camera){

     ShieldObject::camera = camera ;
     shield = LoadTexture("../resources/objects/shield_magic.png");
     recupItem = false ; 
     son = -1 ; 
    shieldSound1 = LoadSound("../resources/sound/itemShield1.wav");
    shieldSound2 = LoadSound("../resources/sound/itemShield2.wav");
    shieldSound3 = LoadSound("../resources/sound/itemShield3.wav");
}

void ShieldObject::Render(Vector3 posDead){

    Vector3 cameraPos = (*camera).position;
    distJoueur = sqrt(pow(posDead.x - cameraPos.x, 2) + pow(posDead.z - cameraPos.z, 2));
    if (distJoueur < 1.0f){
        son = GetRandomValue(1,3) ;
        if(son == 1) PlaySound(shieldSound1);
        if(son == 2) PlaySound(shieldSound2);
        if(son == 3) PlaySound(shieldSound3);

        recupItem = true;
    }
if (recupItem == true){
    if(shieldJoueurTest < shieldJoueurMaxTest) shieldJoueurTest = shieldJoueurTest + 15 ;
}

if (recupItem != true){
    DrawCylinder((Vector3){posDead.x, 0.0f, posDead.z}, 0.5f/8.0f, 0.5f/8.0f, 0.0001f, 16, DARKGRAY);
    DrawBillboard(*camera, shield, posDead, 0.2f/2.0f ,WHITE);
}}



