#include "raylib.h"
#include "item.h"

#ifndef ENNEMI_H
#define ENNEMI_H

class Ennemi
{
    public:
        bool dead;
        int checkDrop ; 
        Vector3 position;
        Item item ; 
        Vector3 posDead ; 
        float distJoueur;
        float taille;
        int pv;
        int *shieldJoueur ; 
        int typeItem ; 
        int tagEnnemiPerso ; 
        int itemType ; 
        static int nbKill;
        static int tagEnnemi ; 
        bool dropItem ; 

        void Init(Color *mapCouleurs, Texture2D dimensionsMap, Vector3 mapPosition, Camera *camera,
                  int *pvJoueur, Texture2D *listeTextures, int *shieldJoueur, int *shieldJoueurMax);

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
        int *shieldJoueurMax ; 

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
        int TagEnnemi(); 
        void CorrigerDeplacementMurs();

};

#endif