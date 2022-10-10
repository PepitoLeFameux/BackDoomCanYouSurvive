#include "raylib.h"

class Ennemi
{
    public:
        bool dead;
        Vector3 position;
        float distJoueur;

        void Init(Color *mapCouleurs, Texture2D dimensionsMap, Vector3 mapPosition, Camera *camera,
                  int *pvJoueur, Texture2D *listeTextures);
        void Action();
        void Damaged(int dmg);
        void Render();

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
        int pv;
        float vitesse;
        int degats;

        float taille;
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
        bool VisionDirecte(Vector3 pos1, Vector3 pos2);
        float DistanceHorizontale(Vector3 pos1, Vector3 pos2);
        void CourtVersJoueur();
        bool CourtVersDestination();
        void CorrigerDeplacementMurs();
        bool CheckCollisionLineCircle(Vector2 startPos, Vector2 endPos, Vector2 center,
                                      float radius, Vector2 *pointCollision);

};