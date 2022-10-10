#include "raylib.h"

#ifndef ENNEMI_H
#define ENNEMI_H

class Ennemi
{
    public:
        bool dead;
        Vector3 position;
        float distJoueur;
        
        float taille;
        int pv;

        void Init(Color *mapCouleurs, Texture2D dimensionsMap, Vector3 mapPosition, Camera *camera,
                  int *pvJoueur, Texture2D *listeTextures);
        void Action();
        void Damaged(int dmg);
        void Render();

        bool VisionDirecte(Vector3 pos1);
        bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center,
                                      float radius, Vector2 *pointCollision);

    private:
        // Entrées
        Color *mapCouleurs;
        Texture2D dimensionsMap;
        Vector3 mapPosition;
        Camera *camera;
        int *pvJoueur;

        Vector3 anciennePosition;

        int typeEnnemi;
        int pvMax;
        float vitesse;
        int degats;

        float distanceCollision;
        float defaultY;
        Texture2D *listeTextures;

        bool poursuite;
        bool actionEnCours;
        double tempsDerniereAction;

        int mode;
        Vector3 destination;
        double debutAttente;
        double dureeAttente;

        double derniereAttaque;


        void SetRandomType(void);
        void SetRandomPos();
        Vector3 GetRandomPos();
        float DistanceHorizontale(Vector3 pos1, Vector3 pos2);
        void CourtVersJoueur();
        bool CourtVersDestination();
        void CorrigerDeplacementMurs();

};

#endif