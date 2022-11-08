#include "item.h"
#include "raylib.h"
#include "cmath"
#include "ath.h"
#include <cstdio>
#include "arme.h"
#include "iostream"


int Item::shieldJoueurTest = 0;
int Item::shieldJoueurMaxTest = 150;
int Item::time = 0;
int Item::previousTime = 0;
bool Item::instaKillEffect = false ; 

void Item::Init(Camera *camera,int  typeItem){
    Item::camera = camera ; 
    recupItem = false;

    if(typeItem == 1){ //shieldItem
        textureItem = LoadTexture("../resources/objects/shield_magic.png");

        son = GetRandomValue(1,3);
        if (son == 1) soundItem = LoadSound("../resources/sound/itemShield1.wav");
        if (son == 2) soundItem = LoadSound("../resources/sound/itemShield2.wav");
        if (son == 3) soundItem = LoadSound("../resources/sound/itemShield3.wav");
    }

    if (typeItem == 2 || typeItem == 3 || typeItem == 4){ //AmmoItem
        textureItem = LoadTexture("../resources/objects/ammo.png");
        soundItem = LoadSound("../resources/sound/ammoItem.wav");
    }

    if (typeItem == 5){ //InstaKill
        textureItem = LoadTexture("../resources/objects/skull.png");
        soundItem = LoadSound("../resources/sound/instaKill.wav");
    }
}

void Item::Render(Vector3 posDead, int typeItem){

    cameraPos = (*camera).position;
    distJoueur = sqrt(pow(posDead.x - cameraPos.x, 2) + pow(posDead.z - cameraPos.z, 2));

    if (distJoueur < 1.0f){
        PlaySound(soundItem);
        recupItem = true;
    }

        if(typeItem == 1 && recupItem == true){
        if (shieldJoueurTest < shieldJoueurMaxTest) shieldJoueurTest = shieldJoueurTest + 15;
    }

        if(typeItem == 2 || typeItem == 3 || typeItem == 4){
            Arme arme ;
            arme.ammoGun = arme.ammoGun + 26 ; 
        }
        
        if (recupItem != true){
        DrawCylinder((Vector3){posDead.x, 0.0f, posDead.z}, 0.5f / 8.0f, 0.5f / 8.0f, 0.0001f, 16, DARKGRAY);
        DrawBillboard(*camera, textureItem, posDead, 0.2f / 2.0f, WHITE);
    }
}

void Item::InstaKill(){
    time = GetTime();
    if (previousTime == 0) previousTime = time ; 
    if(time - previousTime < 20){
        instaKillEffect = true ;
        }
    if(time - previousTime > 20){
        instaKillEffect = false ;
        previousTime = 0 ; 
        }
}