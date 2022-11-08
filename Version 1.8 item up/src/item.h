#include "raylib.h"

#ifndef OBJECTS_H
#define OBJECTS_H

class Item{

    public:

    int son ;
    bool recupItem ;
    static bool instaKillEffect ; 
    bool instaKillBegin ; 
    int distJoueur ;
    static int time ;
    static int previousTime ;
        
    static int shieldJoueurTest;
    static int shieldJoueurMaxTest;

    Texture2D  textureItem ;
    Vector3 cameraPos ; 

    Sound soundItem ;

    void Init(Camera *camera, int typeItem);
    void Render(Vector3 posDead, int typeItem);
    void InstaKill();

    private :
    Camera *camera ;


};


#endif