#include "raylib.h"
#include "unordered_map"
#include "projectile.h"
#include "vector"

#ifndef ENNEMI_H
#define ENNEMI_H

class Ennemi
{
    public:
        bool operator<(Ennemi const &) const;
        bool operator>(Ennemi const &) const;
        bool operator==(Ennemi const *) const;
        std::vector<Ennemi> *ennemis;
        bool dead;
        double deathTime;
        Vector3 position;
        Vector3 deadpos = (Vector3){5000.0f, 0.0f, 5000.0f};
        float distJoueur;
        float taille;
        static int argent ; 
        int pv;
        int money ; 
        int *shieldJoueur; 
        static int nbKill;
        bool itemState;
        std::string itemType;
        int *ammo;
        int *getAmmo;
        int *clip;
        bool *unlocked;
        bool dmged;
        double dmgTime;

        void Init(std::vector<Ennemi> *ennemis, Color *mapCouleurs, Texture2D dimensionsMap, Vector3 mapPosition, Camera *camera,
                  int *pvJoueur, Texture2D *listeTextures, int *shieldJoueur, int shieldJoueurMax,
                  int *ennemisRestants, std::unordered_map<std::string, bool> *newEffects, int *ammo,
                  int *clip, bool* unlocked, int *getAmmo);
        void Spawn();

        void Action();
        void Damaged(int dmg);
        void Render();

        bool VisionDirecte(Vector3 pos1);
        bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center,
                                      float radius, Vector2 *pointCollision);

        Sound instaKillSound ; 
        Sound shieldSound ; 
        Sound ammoSound ; 
        Sound maxAmmo ; 
        Sound radar;


    private:
        // Entrées
        Color *mapCouleurs;
        Texture2D dimensionsMap;
        Vector3 mapPosition;
        Camera *camera;
        int *pvJoueur;
        std::unordered_map<std::string, bool> *newEffects;

        Vector3 anciennePosition;

        int typeEnnemi;
        int *ennemisRestants;
        int pvMax;
        float vitesse;
        int degats;
        int shieldJoueurMax ; 

        float distanceCollision;
        float defaultY;
        Texture2D *listeTextures;
        float decal;
        Texture2D itemStateTexture;

        bool poursuite;
        bool actionEnCours;
        double tempsDerniereAction;

        int mode;
        Vector3 destination;
        double debutAttente;
        double dureeAttente;

        double derniereAttaque;


        void SetRandomType();
        void SetRandomPos();
        Vector3 GetRandomPos();
        float DistanceHorizontale(Vector3 pos1, Vector3 pos2);
        void CourtVersJoueur();
        bool CourtVersDestination();
        int TagEnnemi(); 
        void CorrigerDeplacementMursEtEnnemis();
        void DeathSequence();

};

#endif