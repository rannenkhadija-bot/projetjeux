#ifndef FONCTION_H
#define FONCTION_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#define MAX_QUESTIONS 50
#define MAX_CHOIX 4
#define TEMPS_PAR_QUESTION 10.0f
#define LARGEUR_ECRAN 1024
#define HAUTEUR_ECRAN 768

typedef struct {
    char question[256];
    char choix[MAX_CHOIX][100];
    int bonneReponse;
    int dejaVu;
} Question;

typedef struct {
    char message[256];
    float angle;
    float scale;
    float alpha;
    bool actif;
    Uint32 duree;
    Uint32 debutTemps;
    bool succes;
} AnimationRotozoom;

typedef struct {
    TTF_Font *font;
    TTF_Font *fontTitre;
    TTF_Font *fontQuestion;
    SDL_Color couleurTexte;
    SDL_Color couleurErreur;
    SDL_Color couleurSucces;
    SDL_Color couleurBouton;
    SDL_Color couleurBoutonHover;
    AnimationRotozoom animation;
    SDL_Texture *backgroundTexture;
    SDL_Texture *heartTexture;
    SDL_Texture *backgroundImage;
    bool useImageBackground;
} Interface;

typedef struct {
    SDL_Rect rect;
    char texte[100];
    bool hover;
    int id;
} Bouton;

void chargerQuestions(const char *filename, Question *questions, int *nbQuestions);
int choisirQuestion(Question *questions, int nbQuestions);
void verifierReponse(int indexQuestion, int choixJoueur, int *score, int *vies, int *level, int *questionsRepondues);
void afficherQuestion(SDL_Renderer *renderer, Interface *ui, Question q);
void miseAJourTemps(float *tempsRestant, float deltaTime, int *vies);
void afficherMessageRotozoom(SDL_Renderer *renderer, Interface *ui, const char *message, bool succes);
void sousMenuEnigme(SDL_Renderer *renderer, Interface *ui, Question *questions, int nbQuestions, int *score, int *vies, int level);
void afficherChronometreBarre(SDL_Renderer *renderer, int x, int y, int largeur, int hauteur, float pourcentage);
void afficherScoreVies(SDL_Renderer *renderer, Interface *ui, int score, int vies, int level);
void afficherMenuPrincipal(SDL_Renderer *renderer, Interface *ui);
void initInterface(SDL_Renderer *renderer, Interface *ui);
void cleanupInterface(SDL_Renderer *renderer, Interface *ui);
void afficherBackground(SDL_Renderer *renderer, Interface *ui);
void afficherBoutonsReponse(SDL_Renderer *renderer, Interface *ui, Bouton boutons[], int nbBoutons);
void initialiserBoutons(Bouton boutons[], Question q);
int gererClicBoutons(Bouton boutons[], int nbBoutons, int x, int y);
void dessinerBordureSciFi(SDL_Renderer *renderer, SDL_Rect rect, int epaisseur, SDL_Color couleur);
void afficherVies(SDL_Renderer *renderer, Interface *ui, int vies);
void dessinerCercle(SDL_Renderer *renderer, int cx, int cy, int rayon, SDL_Color couleur);
void dessinerArc(SDL_Renderer *renderer, int cx, int cy, int rayon, int angleDebut, int angleFin, SDL_Color couleur);
void chargerBackgroundImage(SDL_Renderer *renderer, Interface *ui, const char *filename);
void afficherTexte(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int x, int y, SDL_Color couleur);

#endif
