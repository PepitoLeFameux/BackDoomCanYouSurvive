#include "raylib.h"
#include "iostream"
#include "arme.h"
#include "vector"

#ifndef PROJECTILE_H
#define PROJECTILE_H


class Projectile
{
    public:
    bool active;
    bool detonated;

    Vector3 position;
    Vector3 direction;
    int animFrame;
    int *frameCounter;
    int previousFrame;
    double previousTime;
    Sound explosionSound ;

    float speed;
    float radius;
    float taille;
    int damage;
    std::string affiliation;

    int *dureeFrames;
    int totalFrames;
    Texture2D *textures;

    int dureeFramesRoquette[15] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
    int totalFramesRoquette = 15;
    Texture2D *texturesRoquette;

    Camera *camera;
    int largeur;
    int hauteur;
    Color *mapCouleurs;
    Texture2D dimensionsMap;
    Vector3 mapPosition;
    std::vector<Ennemi> *listeEnnemis;
    Projectile *listeProjectiles;
    int nbProjectilesMax;

    void Init(std::vector<Ennemi> *listeEnnemis, Color *mapCouleurs, Texture2D dimensionsMap,
              Vector3 mapPosition, Projectile *listeProjectiles, int nbProjectilesMax, int *frameCounter,
              Texture2D *texturesRoquette, Camera *camera, int largeur, int hauteur);
    void Launch(Vector3 position, Vector3 direction, std::string type);
    void Action();
    void Detonate();
    void Render();

    void CheckCollisionMurs();
    void CheckCollisionEnnemis();
    bool hit;
    bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center,
                                      float radius, Vector2 *pointCollision);
};

#endif