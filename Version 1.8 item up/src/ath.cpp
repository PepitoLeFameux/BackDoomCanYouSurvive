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
    instaKill = LoadTexture("../resources/objects/skull.png");
    heart = heart1 ; 
    gameOverScreen = LoadTexture("../resources/ath/gameoverscreen.png");
    amogusScreen = LoadTexture("../resources/ath/amogusScreen.png");
    player = LoadTexture("../resources/ennemis/1.png");
}

void Ath::startRender(){
    
}

void Ath::gameOverRender(){
    Rectangle rec_gameOver = {0,0,gameOverScreen.width, gameOverScreen.height};
    Rectangle rec_gameOverScreen = {0,0, largeurEcran, hauteurEcran};
    Rectangle rec_amogus = {0,0,amogusScreen.width, amogusScreen.height};
    Rectangle rec_amogusdest = {(largeurEcran - rec_amogusdest.width)/2,(hauteurEcran - rec_amogusdest.height)/2,650,500};

    DrawTexturePro(gameOverScreen, rec_gameOver, rec_gameOverScreen, (Vector2){0,0}, 0.0f, BLACK);
    DrawTexturePro(amogusScreen, rec_amogus, rec_amogusdest, (Vector2){0,0}, 0.0f, WHITE);

    std::string strKill = "Nombre de Kills : " + std::to_string(*nbKill) ;
    DrawText(strKill.c_str(), (largeurEcran/2)- 150 ,920,35,RED);

    std::string strStart = "Appuyez sur Entrée pour relancer une partie !" ;
    DrawText(strStart.c_str(), 500, 1000, 40, YELLOW);

    if(IsKeyDown(KEY_E)){
        DrawTexturePro(gameOverScreen, rec_gameOver, rec_gameOverScreen, (Vector2){0,0}, 0.0f, WHITE);
    }

}

void Ath::Render(){

    // Affichage des munitions + arme
    Rectangle rec_texture = {0,0,cadreAthArmes.width,cadreAthArmes.height};
    Rectangle rec_ath = {0,930,largeurEcran/4,1080 - 930};

    std::string StrAmmo = std::to_string(arme->clipAmmo[arme->numeroArme]) + " / " + std::to_string(arme->clip[arme->numeroArme]);
    std::string StrAmmo2 = std::to_string(arme->ammo[arme->numeroArme]);

    DrawTexturePro(cadreAthArmes,rec_texture,rec_ath,(Vector2){0,0},0.0f,WHITE);
    DrawRectangleLines(0,930,largeurEcran/4,1080 - 930,WHITE);
    DrawLine(300,930,300,1080,WHITE);

    DrawText(StrAmmo.c_str(),340, 950, 30, YELLOW );
    DrawText(StrAmmo2.c_str(), 340, 1000, 30, YELLOW);

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

    Rectangle rec_texture_instaKill = {0,0,instaKill.width, instaKill.height} ; 
    Rectangle rec_texture_instaKill_dest = {1840,2, 80, 80} ;
    if (item.instaKillEffect == true) DrawTexturePro(instaKill, rec_texture_instaKill, rec_texture_instaKill_dest,(Vector2){0,0}, 0.0f,  WHITE);

}