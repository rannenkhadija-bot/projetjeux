// fichier.c
#include "fichier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

void initInterface(SDL_Renderer *renderer, Interface *ui) {
    ui->font = TTF_OpenFont("assets/Orbitron.ttf", 20);
    ui->fontTitre = TTF_OpenFont("assets/Orbitron.ttf", 32);
    ui->fontQuestion = TTF_OpenFont("assets/Orbitron.ttf", 24);
    
    if (!ui->font) ui->font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 20);
    if (!ui->fontTitre) ui->fontTitre = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 32);
    if (!ui->fontQuestion) ui->fontQuestion = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 24);
    
    ui->couleurTexte = (SDL_Color){0, 255, 255, 255};
    ui->couleurErreur = (SDL_Color){255, 0, 0, 255};
    ui->couleurSucces = (SDL_Color){0, 255, 0, 255};
    ui->couleurBouton = (SDL_Color){0, 100, 150, 200};
    ui->couleurBoutonHover = (SDL_Color){0, 150, 200, 255};
    
    ui->animation.actif = false;
    
    ui->backgroundTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LARGEUR_ECRAN, HAUTEUR_ECRAN);
    SDL_SetRenderTarget(renderer, ui->backgroundTexture);
    
    for (int i = 0; i < HAUTEUR_ECRAN; i++) {
        int color = 20 + (i * 80 / HAUTEUR_ECRAN);
        SDL_SetRenderDrawColor(renderer, 0, 0, color, 255);
        SDL_RenderDrawLine(renderer, 0, i, LARGEUR_ECRAN, i);
    }
    
    srand(time(NULL));
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 200; i++) {
        int x = rand() % LARGEUR_ECRAN;
        int y = rand() % HAUTEUR_ECRAN;
        SDL_RenderDrawPoint(renderer, x, y);
    }
    
    SDL_SetRenderTarget(renderer, NULL);
    
    ui->heartTexture = NULL;
    SDL_Surface *heartSurf = IMG_Load("assets/heart.png");
    if (heartSurf) {
        ui->heartTexture = SDL_CreateTextureFromSurface(renderer, heartSurf);
        SDL_FreeSurface(heartSurf);
    }
    
    ui->scoreTexture = NULL;
    SDL_Surface *scoreSurf = IMG_Load("assets/score.png");
    if (scoreSurf) {
        ui->scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurf);
        SDL_FreeSurface(scoreSurf);
    }
    
    ui->defaultBg = NULL;
    SDL_Surface *bgSurf = IMG_Load("assets/background_1.png");
    if (bgSurf) {
        ui->defaultBg = SDL_CreateTextureFromSurface(renderer, bgSurf);
        SDL_FreeSurface(bgSurf);
    }
}

void cleanupInterface(Interface *ui) {
    if (ui->font) TTF_CloseFont(ui->font);
    if (ui->fontTitre) TTF_CloseFont(ui->fontTitre);
    if (ui->fontQuestion) TTF_CloseFont(ui->fontQuestion);
    if (ui->backgroundTexture) SDL_DestroyTexture(ui->backgroundTexture);
    if (ui->heartTexture) SDL_DestroyTexture(ui->heartTexture);
    if (ui->scoreTexture) SDL_DestroyTexture(ui->scoreTexture);
    if (ui->defaultBg) SDL_DestroyTexture(ui->defaultBg);
}

void afficherBackground(SDL_Renderer *renderer, Interface *ui, SDL_Texture* customBg) {
    if (customBg) {
        SDL_RenderCopy(renderer, customBg, NULL, NULL);
    } else if (ui->defaultBg) {
        SDL_RenderCopy(renderer, ui->defaultBg, NULL, NULL);
    } else if (ui->backgroundTexture) {
        SDL_RenderCopy(renderer, ui->backgroundTexture, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
}

void afficherTexte(SDL_Renderer *renderer, TTF_Font *font, const char *texte, int x, int y, SDL_Color couleur) {
    if (!font) return;
    
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, texte, couleur);
    if (!surface) return;
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void afficherScoreVies(SDL_Renderer *renderer, Interface *ui, int score, int vies, int level) {
    char buffer[100];
    
    sprintf(buffer, "[ NIVEAU: %d ]", level);
    afficherTexte(renderer, ui->font, buffer, 300, 30, ui->couleurTexte);
    
    afficherScore(renderer, ui, score);
    afficherVies(renderer, ui, vies);
}

void afficherScore(SDL_Renderer *renderer, Interface *ui, int score) {
    if (ui->scoreTexture) {
        SDL_Rect scoreRect = {50, 25, 32, 32};
        SDL_RenderCopy(renderer, ui->scoreTexture, NULL, &scoreRect);
        char buffer[32];
        sprintf(buffer, "%d", score);
        afficherTexte(renderer, ui->font, buffer, 90, 30, ui->couleurTexte);
    } else {
        char buffer[100];
        sprintf(buffer, "[ SCORE: %d ]", score);
        afficherTexte(renderer, ui->font, buffer, 50, 30, ui->couleurTexte);
    }
}

void afficherVies(SDL_Renderer *renderer, Interface *ui, int vies) {
    if (ui->heartTexture) {
        for (int i = 0; i < vies; i++) {
            SDL_Rect heartRect = {LARGEUR_ECRAN - 150 + (i * 35), 25, 30, 30};
            SDL_RenderCopy(renderer, ui->heartTexture, NULL, &heartRect);
        }
    } else {
        char buffer[50] = "VIES: ";
        for (int i = 0; i < vies; i++) {
            strcat(buffer, "♥ ");
        }
        afficherTexte(renderer, ui->font, buffer, LARGEUR_ECRAN - 200, 30, (SDL_Color){255, 0, 0, 255});
    }
}

void afficherQuestion(SDL_Renderer *renderer, Interface *ui, Question q) {
    SDL_Rect questionRect = {150, 120, LARGEUR_ECRAN - 300, 120};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &questionRect);
    dessinerBordureSciFi(renderer, questionRect, 2, (SDL_Color){0, 255, 255, 255});
    
    afficherTexte(renderer, ui->fontQuestion, q.question, 
                  questionRect.x + 20, questionRect.y + 40, ui->couleurTexte);
}

void afficherBoutonsReponse(SDL_Renderer *renderer, Interface *ui, Bouton boutons[], int nbBoutons) {
    for (int i = 0; i < nbBoutons; i++) {
        SDL_Color couleurBouton = boutons[i].hover ? ui->couleurBoutonHover : ui->couleurBouton;
        
        SDL_SetRenderDrawColor(renderer, couleurBouton.r, couleurBouton.g, couleurBouton.b, couleurBouton.a);
        SDL_RenderFillRect(renderer, &boutons[i].rect);
        
        dessinerBordureSciFi(renderer, boutons[i].rect, 1, (SDL_Color){0, 255, 255, 255});
        
        int texteX = boutons[i].rect.x + 20;
        int texteY = boutons[i].rect.y + (boutons[i].rect.h / 2) - 10;
        afficherTexte(renderer, ui->font, boutons[i].texte, texteX, texteY, ui->couleurTexte);
    }
}

void initialiserBoutons(Bouton boutons[], Question q) {
    int largeurBouton = 350;
    int hauteurBouton = 80;
    int espacement = 40;
    int debutX = (LARGEUR_ECRAN - (2 * largeurBouton + espacement)) / 2;
    int debutY = 320;
    
    boutons[0].rect = (SDL_Rect){debutX, debutY, largeurBouton, hauteurBouton};
    strcpy(boutons[0].texte, q.choix[0]);
    boutons[0].hover = false;
    boutons[0].id = 0;
    
    boutons[1].rect = (SDL_Rect){debutX + largeurBouton + espacement, debutY, largeurBouton, hauteurBouton};
    strcpy(boutons[1].texte, q.choix[1]);
    boutons[1].hover = false;
    boutons[1].id = 1;
    
    boutons[2].rect = (SDL_Rect){debutX, debutY + hauteurBouton + espacement, largeurBouton, hauteurBouton};
    strcpy(boutons[2].texte, q.choix[2]);
    boutons[2].hover = false;
    boutons[2].id = 2;
    
    boutons[3].rect = (SDL_Rect){debutX + largeurBouton + espacement, debutY + hauteurBouton + espacement, largeurBouton, hauteurBouton};
    strcpy(boutons[3].texte, q.choix[3]);
    boutons[3].hover = false;
    boutons[3].id = 3;
}

int gererClicBoutons(Bouton boutons[], int nbBoutons, int x, int y) {
    for (int i = 0; i < nbBoutons; i++) {
        if (x >= boutons[i].rect.x && x <= boutons[i].rect.x + boutons[i].rect.w &&
            y >= boutons[i].rect.y && y <= boutons[i].rect.y + boutons[i].rect.h) {
            return boutons[i].id;
        }
    }
    return -1;
}

void afficherChronometreBarre(SDL_Renderer *renderer, int x, int y, int largeur, int hauteur, float pourcentage) {
    SDL_Rect barreFond = {x, y, largeur, hauteur};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &barreFond);
    
    int largeurProgression = (int)(largeur * pourcentage);
    if (largeurProgression > 0) {
        SDL_Rect barreProgression = {x, y, largeurProgression, hauteur};
        
        if (pourcentage > 0.6) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        } else if (pourcentage > 0.3) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &barreProgression);
    }
    
    dessinerBordureSciFi(renderer, barreFond, 1, (SDL_Color){0, 255, 255, 255});
}

void afficherMessageRotozoom(SDL_Renderer *renderer, Interface *ui, const char *message, bool succes) {
    if (!ui->animation.actif) {
        strcpy(ui->animation.message, message);
        ui->animation.angle = 0;
        ui->animation.scale = 0.3f;
        ui->animation.alpha = 255;
        ui->animation.actif = true;
        ui->animation.debutTemps = SDL_GetTicks();
        ui->animation.duree = 800;
        ui->animation.succes = succes;
    }
    
    Uint32 tempsEcoule = SDL_GetTicks() - ui->animation.debutTemps;
    if (tempsEcoule >= ui->animation.duree) {
        ui->animation.actif = false;
        return;
    }
    
    float progression = (float)tempsEcoule / ui->animation.duree;
    ui->animation.angle = 720 * progression;
    ui->animation.scale = 0.3f + progression * 1.2f;
    ui->animation.alpha = 255 * (1 - progression);
    
    SDL_Color couleur = succes ? ui->couleurSucces : ui->couleurErreur;
    SDL_Surface *surface = TTF_RenderUTF8_Blended(ui->fontTitre, message, couleur);
    if (!surface) return;
    
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int w = surface->w, h = surface->h;
    SDL_FreeSurface(surface);
    
    int newW = (int)(w * ui->animation.scale);
    int newH = (int)(h * ui->animation.scale);
    int x = (LARGEUR_ECRAN - newW) / 2;
    int y = (HAUTEUR_ECRAN - newH) / 2;
    
    SDL_Rect dstRect = {x, y, newW, newH};
    SDL_SetTextureAlphaMod(texture, (Uint8)ui->animation.alpha);
    SDL_RenderCopyEx(renderer, texture, NULL, &dstRect, ui->animation.angle, NULL, SDL_FLIP_NONE);
    
    SDL_DestroyTexture(texture);
}

void chargerQuestions(const char *filename, Question *questions, int *nbQuestions, SDL_Renderer *renderer) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        *nbQuestions = 0;
        return;
    }
    
    char ligne[512];
    int index = 0;
    
    while (fgets(ligne, sizeof(ligne), file) && index < MAX_QUESTIONS) {
        ligne[strcspn(ligne, "\n")] = 0;
        if (strlen(ligne) == 0) continue;
        
        strcpy(questions[index].question, ligne);
        
        if (fgets(ligne, sizeof(ligne), file)) {
            ligne[strcspn(ligne, "\n")] = 0;
            char *token = strtok(ligne, ";");
            int choixIndex = 0;
            while (token && choixIndex < MAX_CHOIX) {
                strcpy(questions[index].choix[choixIndex], token);
                token = strtok(NULL, ";");
                choixIndex++;
            }
        }
        
        if (fgets(ligne, sizeof(ligne), file)) {
            questions[index].bonneReponse = atoi(ligne);
        }
        
        questions[index].dejaVu = 0;
        questions[index].backgroundTex = NULL;
        
        char bgPath[256];
        sprintf(bgPath, "assets/ques_%d.png", index + 1);
        SDL_Surface* bgSurf = IMG_Load(bgPath);
        if (bgSurf) {
            questions[index].backgroundTex = SDL_CreateTextureFromSurface(renderer, bgSurf);
            SDL_FreeSurface(bgSurf);
        }
        
        index++;
    }
    
    *nbQuestions = index;
    fclose(file);
}

int choisirQuestion(Question *questions, int nbQuestions) {
    int questionsDisponibles[MAX_QUESTIONS];
    int nbDisponibles = 0;
    
    for (int i = 0; i < nbQuestions; i++) {
        if (!questions[i].dejaVu) {
            questionsDisponibles[nbDisponibles++] = i;
        }
    }
    
    if (nbDisponibles == 0) {
        for (int i = 0; i < nbQuestions; i++) {
            questions[i].dejaVu = 0;
        }
        for (int i = 0; i < nbQuestions; i++) {
            questionsDisponibles[i] = i;
        }
        nbDisponibles = nbQuestions;
    }
    
    int choix = rand() % nbDisponibles;
    return questionsDisponibles[choix];
}

void verifierReponse(int indexQuestion, int choixJoueur, int *score, int *vies, int *level, int *questionsRepondues) {
    if (choixJoueur == indexQuestion) {
        *score += 10;
        *questionsRepondues += 1;
        
        if (*questionsRepondues >= 5) {
            (*level)++;
            *questionsRepondues = 0;
        }
    } else {
        *vies -= 1;
    }
}

void miseAJourTemps(float *tempsRestant, float deltaTime, int *vies) {
    *tempsRestant -= deltaTime;
    if (*tempsRestant <= 0) {
        *vies -= 1;
        *tempsRestant = TEMPS_PAR_QUESTION;
    }
}

void dessinerBordureSciFi(SDL_Renderer *renderer, SDL_Rect rect, int epaisseur, SDL_Color couleur) {
    SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
    for (int i = 0; i < epaisseur; i++) {
        SDL_Rect border = {rect.x - i, rect.y - i, rect.w + 2*i, rect.h + 2*i};
        SDL_RenderDrawRect(renderer, &border);
    }
    
    int tailleCoin = 15;
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    
    SDL_RenderDrawLine(renderer, rect.x - epaisseur, rect.y - epaisseur, 
                       rect.x - epaisseur + tailleCoin, rect.y - epaisseur);
    SDL_RenderDrawLine(renderer, rect.x - epaisseur, rect.y - epaisseur,
                       rect.x - epaisseur, rect.y - epaisseur + tailleCoin);
    SDL_RenderDrawLine(renderer, rect.x + rect.w + epaisseur, rect.y - epaisseur,
                       rect.x + rect.w + epaisseur - tailleCoin, rect.y - epaisseur);
    SDL_RenderDrawLine(renderer, rect.x + rect.w + epaisseur, rect.y - epaisseur,
                       rect.x + rect.w + epaisseur, rect.y - epaisseur + tailleCoin);
    SDL_RenderDrawLine(renderer, rect.x - epaisseur, rect.y + rect.h + epaisseur,
                       rect.x - epaisseur + tailleCoin, rect.y + rect.h + epaisseur);
    SDL_RenderDrawLine(renderer, rect.x - epaisseur, rect.y + rect.h + epaisseur,
                       rect.x - epaisseur, rect.y + rect.h + epaisseur - tailleCoin);
    SDL_RenderDrawLine(renderer, rect.x + rect.w + epaisseur, rect.y + rect.h + epaisseur,
                       rect.x + rect.w + epaisseur - tailleCoin, rect.y + rect.h + epaisseur);
    SDL_RenderDrawLine(renderer, rect.x + rect.w + epaisseur, rect.y + rect.h + epaisseur,
                       rect.x + rect.w + epaisseur, rect.y + rect.h + epaisseur - tailleCoin);
}

void afficherMenuPrincipal(SDL_Renderer *renderer, Interface *ui) {
    afficherBackground(renderer, ui, NULL);
}
