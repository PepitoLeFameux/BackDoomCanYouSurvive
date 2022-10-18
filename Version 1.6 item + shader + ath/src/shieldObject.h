#include "raylib.h"

#ifndef OBJECTS_H
#define OBJECTS_H

class ShieldObject
{
    public :
    
    static int shieldJoueurTest;
    static int shieldJoueurMaxTest;
    int son ; 
    float defaultY ;
    float distJoueur ; 

    bool animItem ; 
    bool recupItem ; 

    Sound shieldSound1 ;
    Sound shieldSound2 ;
    Sound shieldSound3 ;

    Texture2D shield ;

    void Init( Camera *camera);
    void Render(Vector3 posDead);
    bool CheckRecupObject(Vector3 posDead);
    void SetRandomPos();
    void UnloadData();

    private:

    Camera *camera ;

};

#endif