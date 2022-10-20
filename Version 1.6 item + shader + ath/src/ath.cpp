#include "raylib.h"
#include "ath.h"
#include "arme.h"
#include "string"
#include "iostream"
#include "item.h"

void Ath::Init(int largeurEcran, int hauteurEcran, Arme *arme, int *pvJoueur, int *nbKill){
    Ath::largeurEcran = largeurEcran ;
    Ath::hauteurEcran = hauteurEcran ;
    Ath::arme = arme ;
    Ath::pvJoueur = pvJoueur ;
    Ath::nbKill = nbKill ;

    cadreAthArmes = LoadTexture("../resources/ath/texture_ath.png");
    shield = LoadTexture("../resources/objects/shield_magic.png");
    heart1 = LoadTexture("../resources/objects/heart1.png");
    heart2 = LoadTexture("../resources/objects/heart2.png");
    heart3 = LoadTexture("../resources/objects/heart3.png");
    heart = heart1 ; 
    player = LoadTexture("../resources/ennemis/1.png");
}



void Ath::Render(){

    // Affichage des munitions + arme
    Rectangle rec_texture = {0,0,cadreAthArmes.width,cadreAthArmes.height};
    Rectangle rec_ath = {0,930,largeurEcran/4,1080 - 930};

    int ammo = arme->ammo ;
    std::string StrAmmo = std::to_string(ammo) + " / 13";

    DrawTexturePro(cadreAthArmes,rec_texture,rec_ath,(Vector2){0,0},0.0f,WHITE);
    DrawRectangleLines(0,930,largeurEcran/4,1080 - 930,WHITE);
    DrawLine(300,930,300,1080,WHITE);

    DrawText(StrAmmo.c_str(),340, 950, 30, YELLOW );

    // Affichage des PV + shield
    Rectangle rec_texture_heart1 = {0,0,heart1.width,heart1.height};
    Rectangle rec_texture_heart2 = {0,0,heart2.width,heart2.height};
    Rectangle rec_texture_heart3 = {0,0,heart3.width,heart3.height};
    Rectangle rec_texture_heart_dest = {8,843,35,35};
    Rectangle rec_texture_shield = {0,0,shield.width, shield.height};
    Rectangle rec_texture_shield_dest = {5,880,40,40};
    Rectangle rec_PV = {0,830,100,100};

    DrawTexturePro(cadreAthArmes,rec_texture,rec_PV,(Vector2){0,0},0.0f,WHITE);
    DrawTexturePro(shield,rec_texture_shield,rec_texture_shield_dest,(Vector2){0,0}, 0.0f, WHITE);
    DrawTexturePro(heart,rec_texture_heart1,rec_texture_heart_dest,(Vector2){0,0},0.0f,WHITE);
    DrawRectangleLines(rec_PV.x,rec_PV.y,rec_PV.width, rec_PV.height,WHITE);

    std::string strPv = std::to_string(*pvJoueur);

    if (66 < *pvJoueur){
        DrawText(strPv.c_str(), 50, 845, 30, GREEN);
        heart = heart1 ;
        }


    if ((66 > *pvJoueur) && (*pvJoueur > 33)){
        DrawText(strPv.c_str(), 50, 845, 30, YELLOW);
        heart = heart2; }

    if (*pvJoueur<33){
        DrawText(strPv.c_str(), 50, 845, 30, RED);
        heart = heart3 ;}

    Item item ; 
    std::string strShield = std::to_string(item.shieldJoueurTest);
    DrawText(strShield.c_str(),50, 885, 30, SKYBLUE);

    // Affichage perso

    Rectangle rec_texture_perso = {0,0,player.width,player.height};
    //Rectangle rec_perso = {,0}

    // Affichage score et kill

    Rectangle scoreboard = {0,730,100,100};

    DrawTexturePro(cadreAthArmes,rec_texture,scoreboard,(Vector2){0,0},0.0f,WHITE);
    DrawRectangleLines(scoreboard.x,scoreboard.y,scoreboard.width,scoreboard.height,WHITE);

    std::string strKill = "Kills : " + std::to_string(*nbKill) ;
    DrawText(strKill.c_str(), 10,740,20,YELLOW);

}