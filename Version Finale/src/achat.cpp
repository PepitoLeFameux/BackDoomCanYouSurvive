#include "achat.h"
#include "raylib.h"
#include "raymath.h"
#include "arme.h"
#include "iostream"


void Achat::Init(Camera *camera, Arme *arme, bool *unlocked, int *argent, int *ammo, int *getAmmo){

    Achat::argent = argent ;
    Achat::camera = camera ; 
    Achat::unlocked = unlocked ; 
    Achat::arme = arme ; 
    Achat::ammo = ammo;
    Achat::getAmmo = getAmmo;
    for (int num = 0 ; num <= 5 ; num++){
        std::string strTexture = "../resources/ath/weapon" + std::to_string(num + 1) + ".png" ;
        weapon[num] = LoadTexture(strTexture.c_str());
        rec_src[num] = {0,0,(float)(weapon[num]).width, (float)(weapon[num]).height};
    }

    cashRegister = LoadSound("../resources/sound/cashRegister.wav");

}


void Achat::Render(){

    DrawBillboardRec(*camera, weapon[0], rec_src[0], positionAchatListe[0], (Vector2){0.25f,0.20f}, WHITE);
    DrawBillboardRec(*camera, weapon[1], rec_src[1], positionAchatListe[1], (Vector2){0.15f,0.13f}, WHITE);
    DrawBillboardRec(*camera, weapon[2], rec_src[2], positionAchatListe[2], (Vector2){0.25f,0.20f}, WHITE);
    DrawBillboardRec(*camera, weapon[3], rec_src[3], positionAchatListe[3], (Vector2){0.25f,0.20f}, WHITE);
    DrawBillboardRec(*camera, weapon[4], rec_src[4], positionAchatListe[4], (Vector2){0.3f,0.20f}, WHITE);
    DrawBillboardRec(*camera, weapon[5], rec_src[5], positionAchatListe[5], (Vector2){0.2f,0.20f}, WHITE);

}

void Achat::RenderMessage(){

    for (int n=0; n<=5; n++){
        if(achatPossibleListe[n] == true){
            std::string strAchat = "Appuyez sur B pour acheter l'arme (" + std::to_string(prixListe[n]) +" $)";
            if(*argent <= prixListe[n]) DrawText(strAchat.c_str(), 600,950,40,RED);
            if(*argent >= prixListe[n]) DrawText(strAchat.c_str(), 600,950,40, LIME);
        }
    }
}

void Achat::Acheter(){

    Vector3 cameraPos = (*camera).position;
    
    for (int n = 0 ; n <= 5 ; n++){
        distJoueurListe[n] = sqrt(pow(positionAchatListe[n].x - cameraPos.x, 2) + pow(positionAchatListe[n].z - cameraPos.z, 2));
        if(distJoueurListe[n] <= 0.2f) achatPossibleListe[n] = true ; 
        else achatPossibleListe[n] = false ; 

        if(achatPossibleListe[n] == true){
            if(achatPossibleListe[n] == true && IsKeyPressed(KEY_B) && *argent >= prixListe[n]){
                *argent -= prixListe[n] ; 
                PlaySound(cashRegister);
                if(unlocked[n+1] == false){
                    unlocked[n+1] = true ;
                    unlocked[arme->numeroArme] = false ;
                    arme->numeroArme = n+1 ;  
                    ammo[n+1] = getAmmo[n+1];
                }
                else
                {
                    ammo[n+1] += getAmmo[n+1]*3;
                }
            }
        }
    }



}