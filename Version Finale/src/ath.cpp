#include "raylib.h"
#include "raymath.h"
#include "ath.h"
#include "arme.h"
#include "string"
#include "iostream"
#include "unordered_map"
#include "vector"
#include "ennemi.h"
#include "achat.h"

void Ath::Init(Camera *camera, int largeurEcran, int hauteurEcran, Arme *arme, int *pvJoueur, int *shieldJoueur, int *nbKill, std::unordered_map<std::string, bool> *effects, unsigned __int8 *numManche,int *argent, int *ennemisRestants, std::vector<Ennemi> *ennemis){
    Ath::largeurEcran = largeurEcran ;
    Ath::hauteurEcran = hauteurEcran ;
    Ath::camera = camera ; 
    Ath::arme = arme ;
    Ath::pvJoueur = pvJoueur ;
    Ath::shieldJoueur = shieldJoueur;
    Ath::nbKill = nbKill ;
    Ath::effects = effects ;
    Ath::numManche = numManche ; 
    Ath::ennemisRestants = ennemisRestants ;
    Ath::argent = argent ;
    Ath::ennemis = ennemis ;

    cadreAthArmes = LoadTexture("../resources/ath/texture_ath.png");
    Achat achat ; 
    shield = LoadTexture("../resources/items/shield.png");
    heart1 = LoadTexture("../resources/items/heart1.png");
    heart2 = LoadTexture("../resources/items/heart2.png");
    heart3 = LoadTexture("../resources/items/heart3.png");
    instaKill = LoadTexture("../resources/items/skull2.png");
    ammoMax = LoadTexture("../resources/items/infiniteammo.png");
    dollar = LoadTexture("../resources/ath/dollar.png") ; 
    heart = heart1 ; 
    gameOverScreen = LoadTexture("../resources/ath/gameoverscreen.png");
    amogusScreen = LoadTexture("../resources/ath/amogusScreen.png");
    backroom = LoadTexture("../resources/ath/backroom.png");
    player = LoadTexture("../resources/ennemis/1.png");
    miniMap = LoadTexture("../resources/map/map4.png");
    radar = LoadTexture("../resources/items/radar.png");
    police = LoadFont("../resources/alagard.ttf");
    police2 = LoadFont("../resources/EVILDEAD.TTF");
    logoSprint = LoadTexture("../resources/ath/sprint.png");

    for(int n=0; n<9; n++)
    {
        std::string file = "../resources/ath/weapon" + std::to_string(n) + ".png";
        weapons[n] = LoadTexture(file.c_str());
    }
}

void Ath::startRender(){

    Rectangle rec_menu = {0,0,backroom.width, backroom.height};
    Rectangle rec_menuScreen = {0,0,largeurEcran, hauteurEcran};

    DrawTexturePro(backroom, rec_menu, rec_menuScreen, (Vector2){0,0}, 0.0f, WHITE);

    std::string title = "BACKDOOM"; 
    DrawTextPro(police,title.c_str(),(Vector2){580,300},(Vector2){0,0}, 0.0f, 150, 2.0f, BLACK );

    std::string info = "Pour lancer la partie appuyez sur Entree !" ; 
    //DrawText(info.c_str(), 1000, 500, 50, YELLOW);
    DrawTextPro(police2, info.c_str(), (Vector2){400,700}, (Vector2){0,0}, 0.0f, 55, 2.0f, WHITE);

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

    std::string strManche = "Manche atteinte : " + std::to_string(*numManche);
    DrawText(strManche.c_str(), 780,  800 ,40, RED);

    std::string strStart = "Appuyez sur Entrée pour relancer une partie !" ;
    DrawText(strStart.c_str(), 500, 1000, 40, YELLOW);

    if(IsKeyDown(KEY_E)){
        DrawTexturePro(gameOverScreen, rec_gameOver, rec_gameOverScreen, (Vector2){0,0}, 0.0f, WHITE);
    }

}

void Ath::displaySprint(int jauge)
{
    int x = 1800, y = 700, w = 50, h = 300;
    int border = 5;
    float propjauge = (float)jauge/240.0f;
    //Contour
    DrawRectangleLinesEx((Rectangle){x-border, y-border, w+2*border, h+2*border}, border, BLACK);
    //Jauge
    DrawRectangle(x, y+1+h*(1.0f-propjauge), w, h*propjauge, YELLOW);

    Rectangle source = {0, 0, logoSprint.width, logoSprint.height};
    Rectangle dest = {x, y-80, 50, 70};
    DrawTexturePro(logoSprint, source, dest, Vector2Zero(), 0.0f, BLACK);
}

void Ath::Render(){

    // Affichage des munitions + arme
    Rectangle rec_texture = {0,0,cadreAthArmes.width,cadreAthArmes.height};
    Rectangle rec_ath = {0,930,largeurEcran/4,1080 - 930};

    std::string StrAmmo = std::to_string(arme->clipAmmo[arme->numeroArme]) + " / " + std::to_string(arme->clip[arme->numeroArme]);
    std::string StrAmmo2 = std::to_string(arme->ammo[arme->numeroArme]);

    DrawTexturePro(cadreAthArmes,rec_texture,rec_ath,(Vector2){0,0},0.0f,WHITE);
    // float taille = 110.0f/(float)weapons[arme->numeroArme].height;
    // DrawText(std::to_string(taille).c_str(), 100, 100, 30, BLUE);
    Rectangle rectangle_weapon_src = {0,0,weapons[arme->numeroArme].width, weapons[arme->numeroArme].height};
    Rectangle rectangle_weapon_dest = {10,960,260,80};

    DrawTexturePro(weapons[arme->numeroArme], rectangle_weapon_src, rectangle_weapon_dest, (Vector2){0,0}, 0.0f, WHITE);
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
    Rectangle rec_PV = {0,830,210,100};

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

    std::string strShield = std::to_string(*shieldJoueur);
    DrawText(strShield.c_str(),50, 885, 30, SKYBLUE);

    // Affichage perso

    Rectangle rec_texture_perso = {0,0,player.width,player.height};

    // Affichage score et kill

    Rectangle scoreboard = {0,730,100,100};

    DrawTexturePro(cadreAthArmes,rec_texture,scoreboard,(Vector2){0,0},0.0f,WHITE);
    DrawRectangleLines(scoreboard.x,scoreboard.y,scoreboard.width,scoreboard.height,WHITE);

    std::string strKill = "Kills : " + std::to_string(*nbKill) ;
    DrawText(strKill.c_str(), 10,740,20,YELLOW);

    std::string strManche = "Manche : " + std::to_string(*numManche);
    DrawText(strManche.c_str(), 10,775,17,YELLOW);

    Rectangle rec_argent_src = {0,0,dollar.width,dollar.height};
    Rectangle rec_argent_dest = {90,860,35,35};

    DrawTexturePro(dollar, rec_argent_src, rec_argent_dest, (Vector2){0,0}, 0.0f, WHITE);

    std::string strArgent = std::to_string(*argent);
    DrawText(strArgent.c_str(),125,865,30,LIME);

    // Affichage miniMap

    Rectangle rec_miniMap_src = {0,0,miniMap.width, miniMap.height};
    Rectangle rec_miniMap_dest = {2,2,200,200};

    if((*ennemisRestants <= 5)|| (*effects)["radar"] == true){
    DrawTexturePro(miniMap,rec_miniMap_src,rec_miniMap_dest, (Vector2){0,0}, 0.0f, WHITE);
    float camX = camera->position.x;
    float camY = camera->position.z;
    int x = (0.5+camX + miniMap.width/2)*rec_miniMap_dest.width/miniMap.width;
    int y = (0.5+camY + miniMap.height/2)*rec_miniMap_dest.height/miniMap.height;
    DrawRectangle(x, y, 4,4, GREEN );

    
        for(int i = 0 ; i < ennemis->size() ; i++ ){
            if(ennemis->at(i).dead == false){
                float nX = ennemis->at(i).position.x ;
                float nY = ennemis->at(i).position.z ;
                x = (0.5+nX + miniMap.width/2)*rec_miniMap_dest.width/miniMap.width;
                y = (0.5+nY + miniMap.height/2)*rec_miniMap_dest.height/miniMap.height;
                DrawRectangle(x,y,4,4,RED);
            }
        }

        for(int i= 0 ; i<=5 ; i++){

            float aX = achat.positionAchatListe[i].x;
            float aY = achat.positionAchatListe[i].z;
            x = (0.5+aX + miniMap.width/2)*rec_miniMap_dest.width/miniMap.width;
            y = (0.5+aY + miniMap.height/2)*rec_miniMap_dest.height/miniMap.height;
            DrawRectangle(x,y,4,4,SKYBLUE);
        }
    }

    // affichage effet en cours

    Rectangle rec_texture_instaKill = {0,0,instaKill.width, instaKill.height} ; 
    Rectangle rec_texture_instaKill_dest = {1840,2, 80, 80} ;
    Rectangle rec_texture_ammoMax = {0,0,ammoMax.width, ammoMax.height};
    Rectangle rec_texture_ammoMax_dest = {1750,2,80,80};
    Rectangle rec_texture_radar = {0,0,radar.width, radar.height};
    Rectangle rec_radar_dest = {1670,2,80,80};
    if((*effects)["infiniteammo"] == true) DrawTexturePro(ammoMax, rec_texture_ammoMax, rec_texture_ammoMax_dest, (Vector2){0,0}, 0.0f, WHITE);
    if ((*effects)["instakill"] == true) DrawTexturePro(instaKill, rec_texture_instaKill, rec_texture_instaKill_dest,(Vector2){0,0}, 0.0f,  WHITE);
    if ( (*effects)["radar"] == true) DrawTexturePro(radar,rec_texture_radar, rec_radar_dest, (Vector2){0,0}, 0.0f, WHITE);

    // Affichage Manches
    if(*numManche%5 == 0 && *numManche != 0){
        temps = GetTime();
        if(tempsPrec == 0) tempsPrec = temps ; 
        if(temps - tempsPrec <= 5){
            std::string strManche = "MANCHE " + std::to_string(*numManche) ; 
            DrawTextPro(police2,strManche.c_str(),(Vector2){760,100},(Vector2){0,0}, 0.0f, 100, 2.0f, RED );
        }
    }

}