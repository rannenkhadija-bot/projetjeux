#include "fonction.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Fonction pour dessiner un cercle (implémentation manuelle)
void dessinerCercle(SDL_Renderer *renderer, int cx, int cy, int rayon, SDL_Color couleur) {
    SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
    
    for (int x = -rayon; x <= rayon; x++) {
        int y = (int)sqrt(rayon * rayon - x * x);
        SDL_RenderDrawLine(renderer, cx + x, cy - y, cx + x, cy + y);
    }
}

// Fonction pour dessiner un arc de cercle
void dessinerArc(SDL_Renderer *renderer, int cx, int cy, int rayon, int angleDebut, int angleFin, SDL_Color couleur) {
    SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
    
    for (int angle = angleDebut; angle <= angleFin; angle += 5) {
        float rad = angle * M_PI / 180.0f;
        int x = cx + (int)(cos(rad) * rayon);
        int y = cy + (int)(sin(rad) * rayon);
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

void initInterface(SDL_Renderer *renderer, Interface *ui) {
    // Chargement des polices
    ui->font = TTF_OpenFont("assets/Orbitron.ttf", 20);
    ui->fontTitre = TTF_OpenFont("assets/Orbitron.ttf", 32);
    ui->fontQuestion = TTF_OpenFont("assets/Orbitron.ttf", 24);
    
    // Polices de secours
    if (!ui->font) ui->font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 20);
    if (!ui->fontTitre) ui->fontTitre = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 32);
    if (!ui->fontQuestion) ui->fontQuestion = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 24);
    
    // Couleurs sci-fi
    ui->couleurTexte = (SDL_Color){0, 255, 255, 255};      // Cyan
    ui->couleurErreur = (SDL_Color){255, 0, 0, 255};       // Rouge
    ui->couleurSucces = (SDL_Color){0, 255, 0, 255};       // Vert
    ui->couleurBouton = (SDL_Color){0, 100, 150, 200};     // Bleu cyan
    ui->couleurBoutonHover = (SDL_Color){0, 150, 200, 255}; // Cyan clair
    
    ui->animation.actif = false;
    
    // Création du fond dégradé sci-fi
    ui->backgroundTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, LARGEUR_ECRAN, HAUTEUR_ECRAN);
    SDL_SetRenderTarget(renderer, ui->backgroundTexture);
    
    // Dégradé du noir vers bleu foncé
    for (int i = 0; i < HAUTEUR_ECRAN; i++) {
        int color = 20 + (i * 80 / HAUTEUR_ECRAN);
        SDL_SetRenderDrawColor(renderer, 0, 0, color, 255);
        SDL_RenderDrawLine(renderer, 0, i, LARGEUR_ECRAN, i);
    }
    
    // Ajout d'étoiles
    srand(time(NULL));
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 200; i++) {
        int x = rand() % LARGEUR_ECRAN;
        int y = rand() % HAUTEUR_ECRAN;
        SDL_RenderDrawPoint(renderer, x, y);
    }
    
    SDL_SetRenderTarget(renderer, NULL);
    
    // Chargement du coeur
    ui->heartTexture = NULL;
    SDL_Surface *heartSurf = IMG_Load("assets/heart.png");
    if (heartSurf) {
        ui->heartTexture = SDL_CreateTextureFromSurface(renderer, heartSurf);
        SDL_FreeSurface(heartSurf);
    }
}

void cleanupInterface(SDL_Renderer *renderer, Interface *ui) {
    if (ui->font) TTF_CloseFont(ui->font);
    if (ui->fontTitre) TTF_CloseFont(ui->fontTitre);
    if (ui->fontQuestion) TTF_CloseFont(ui->fontQuestion);
    if (ui->backgroundTexture) SDL_DestroyTexture(ui->backgroundTexture);
    if (ui->heartTexture) SDL_DestroyTexture(ui->heartTexture);
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

void afficherBackground(SDL_Renderer *renderer, Interface *ui) {
    if (ui->backgroundTexture) {
        SDL_RenderCopy(renderer, ui->backgroundTexture, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
}

void dessinerBordureSciFi(SDL_Renderer *renderer, SDL_Rect rect, int epaisseur, SDL_Color couleur) {
    // Bordure externe
    SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
    for (int i = 0; i < epaisseur; i++) {
        SDL_Rect border = {rect.x - i, rect.y - i, rect.w + 2*i, rect.h + 2*i};
        SDL_RenderDrawRect(renderer, &border);
    }
    
    // Coins décoratifs
    int tailleCoin = 15;
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    
    // Coin haut-gauche
    SDL_RenderDrawLine(renderer, rect.x - epaisseur, rect.y - epaisseur, 
                       rect.x - epaisseur + tailleCoin, rect.y - epaisseur);
    SDL_RenderDrawLine(renderer, rect.x - epaisseur, rect.y - epaisseur,
                       rect.x - epaisseur, rect.y - epaisseur + tailleCoin);
    
    // Coin haut-droit
    SDL_RenderDrawLine(renderer, rect.x + rect.w + epaisseur, rect.y - epaisseur,
                       rect.x + rect.w + epaisseur - tailleCoin, rect.y - epaisseur);
    SDL_RenderDrawLine(renderer, rect.x + rect.w + epaisseur, rect.y - epaisseur,
                       rect.x + rect.w + epaisseur, rect.y - epaisseur + tailleCoin);
    
    // Coin bas-gauche
    SDL_RenderDrawLine(renderer, rect.x - epaisseur, rect.y + rect.h + epaisseur,
                       rect.x - epaisseur + tailleCoin, rect.y + rect.h + epaisseur);
    SDL_RenderDrawLine(renderer, rect.x - epaisseur, rect.y + rect.h + epaisseur,
                       rect.x - epaisseur, rect.y + rect.h + epaisseur - tailleCoin);
    
    // Coin bas-droit
    SDL_RenderDrawLine(renderer, rect.x + rect.w + epaisseur, rect.y + rect.h + epaisseur,
                       rect.x + rect.w + epaisseur - tailleCoin, rect.y + rect.h + epaisseur);
    SDL_RenderDrawLine(renderer, rect.x + rect.w + epaisseur, rect.y + rect.h + epaisseur,
                       rect.x + rect.w + epaisseur, rect.y + rect.h + epaisseur - tailleCoin);
}

void afficherScoreVies(SDL_Renderer *renderer, Interface *ui, int score, int vies, int level) {
    char buffer[100];
    
    // Score avec icône style sci-fi
    sprintf(buffer, "[ SCORE: %d ]", score);
    afficherTexte(renderer, ui->font, buffer, 50, 30, ui->couleurTexte);
    
    // Niveau
    sprintf(buffer, "[ NIVEAU: %d ]", level);
    afficherTexte(renderer, ui->font, buffer, 300, 30, ui->couleurTexte);
    
    // Vies avec coeurs
    afficherVies(renderer, ui, vies);
}

void afficherVies(SDL_Renderer *renderer, Interface *ui, int vies) {
    if (ui->heartTexture) {
        for (int i = 0; i < vies; i++) {
            SDL_Rect heartRect = {LARGEUR_ECRAN - 150 + (i * 35), 25, 30, 30};
            SDL_RenderCopy(renderer, ui->heartTexture, NULL, &heartRect);
        }
    } else {
        // Fallback si pas d'image
        char buffer[50] = "VIES: ";
        for (int i = 0; i < vies; i++) {
            strcat(buffer, "♥ ");
        }
        afficherTexte(renderer, ui->font, buffer, LARGEUR_ECRAN - 200, 30, (SDL_Color){255, 0, 0, 255});
    }
}

void afficherQuestion(SDL_Renderer *renderer, Interface *ui, Question q) {
    // Cadre de la question avec style sci-fi
    SDL_Rect questionRect = {150, 120, LARGEUR_ECRAN - 300, 120};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &questionRect);
    dessinerBordureSciFi(renderer, questionRect, 2, (SDL_Color){0, 255, 255, 255});
    
    // Texte de la question
    afficherTexte(renderer, ui->fontQuestion, q.question, 
                  questionRect.x + 20, questionRect.y + 40, ui->couleurTexte);
}

void afficherBoutonsReponse(SDL_Renderer *renderer, Interface *ui, Bouton boutons[], int nbBoutons) {
    for (int i = 0; i < nbBoutons; i++) {
        // Couleur du bouton selon hover
        SDL_Color couleurBouton = boutons[i].hover ? ui->couleurBoutonHover : ui->couleurBouton;
        
        // Fond du bouton avec transparence
        SDL_SetRenderDrawColor(renderer, couleurBouton.r, couleurBouton.g, couleurBouton.b, couleurBouton.a);
        SDL_RenderFillRect(renderer, &boutons[i].rect);
        
        // Bordure sci-fi
        dessinerBordureSciFi(renderer, boutons[i].rect, 1, (SDL_Color){0, 255, 255, 255});
        
        // Texte du bouton
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
    
    // Bouton 1 (haut-gauche)
    boutons[0].rect = (SDL_Rect){debutX, debutY, largeurBouton, hauteurBouton};
    strcpy(boutons[0].texte, q.choix[0]);
    boutons[0].hover = false;
    boutons[0].id = 0;
    
    // Bouton 2 (haut-droit)
    boutons[1].rect = (SDL_Rect){debutX + largeurBouton + espacement, debutY, largeurBouton, hauteurBouton};
    strcpy(boutons[1].texte, q.choix[1]);
    boutons[1].hover = false;
    boutons[1].id = 1;
    
    // Bouton 3 (bas-gauche)
    boutons[2].rect = (SDL_Rect){debutX, debutY + hauteurBouton + espacement, largeurBouton, hauteurBouton};
    strcpy(boutons[2].texte, q.choix[2]);
    boutons[2].hover = false;
    boutons[2].id = 2;
    
    // Bouton 4 (bas-droit)
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
    // Fond de la barre
    SDL_Rect barreFond = {x, y, largeur, hauteur};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &barreFond);
    
    // Barre de progression
    int largeurProgression = (int)(largeur * pourcentage);
    if (largeurProgression > 0) {
        SDL_Rect barreProgression = {x, y, largeurProgression, hauteur};
        
        // Couleur selon le temps restant
        if (pourcentage > 0.6) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        } else if (pourcentage > 0.3) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &barreProgression);
    }
    
    // Bordure sci-fi
    dessinerBordureSciFi(renderer, barreFond, 1, (SDL_Color){0, 255, 255, 255});
    
    // Texte du temps
    char buffer[20];
    sprintf(buffer, "%.0f sec", pourcentage * TEMPS_PAR_QUESTION);
    afficherTexte(renderer, TTF_OpenFont("assets/Orbitron.ttf", 16), buffer, 
                  x + largeur + 20, y + 5, (SDL_Color){0, 255, 255, 255});
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

void chargerQuestions(const char *filename, Question *questions, int *nbQuestions) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: Impossible d'ouvrir %s\n", filename);
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
        index++;
    }
    
    *nbQuestions = index;
    fclose(file);
    printf("Chargé %d questions\n", *nbQuestions);
}

int choisirQuestion(Question *questions, int nbQuestions) {
    int questionsDisponibles[MAX_QUESTIONS];
    int nbDisponibles = 0;
    
    for (int i = 0; i < nbQuestions; i++) {
        if (!questions[i].dejaVu) {
            questionsDisponibles[nbDisponibles++] = i;
        }
    }
    
    if (nbDisponibles == 0) return -1;
    
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

void sousMenuEnigme(SDL_Renderer *renderer, Interface *ui, Question *questions, int nbQuestions, int *score, int *vies, int level) {
    bool enCours = true;
    SDL_Event event;
    int choix = -1;
    
    while (enCours) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                enCours = false;
                exit(0);
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_1) choix = 0;
                else if (event.key.keysym.sym == SDLK_2) choix = 1;
                else if (event.key.keysym.sym == SDLK_ESCAPE) enCours = false;
            }
        }
        
        SDL_RenderClear(renderer);
        afficherBackground(renderer, ui);
        
        SDL_Rect menuRect = {200, 150, LARGEUR_ECRAN - 400, HAUTEUR_ECRAN - 300};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer, &menuRect);
        dessinerBordureSciFi(renderer, menuRect, 2, (SDL_Color){0, 255, 255, 255});
        
        afficherTexte(renderer, ui->fontTitre, "=== MODE ENIGME ===", 350, 180, ui->couleurTexte);
        afficherTexte(renderer, ui->font, "Pour l'equipe de 6 joueurs:", 250, 280, ui->couleurTexte);
        afficherTexte(renderer, ui->font, "- Mode QCM classique", 270, 330, ui->couleurTexte);
        afficherTexte(renderer, ui->font, "Pour l'equipe de 5 joueurs:", 250, 400, ui->couleurTexte);
        afficherTexte(renderer, ui->font, "- Mode Puzzle (questions bonus)", 270, 450, ui->couleurTexte);
        afficherTexte(renderer, ui->font, "Appuyez sur 1 ou 2 pour choisir", 280, 550, ui->couleurTexte);
        afficherTexte(renderer, ui->font, "ESC pour retourner au jeu", 300, 600, ui->couleurTexte);
        
        if (choix == 0) {
            afficherTexte(renderer, ui->font, "Mode QCM selectionne!", 350, 650, ui->couleurSucces);
            SDL_RenderPresent(renderer);
            SDL_Delay(1000);
            enCours = false;
        }
        else if (choix == 1) {
            afficherTexte(renderer, ui->font, "Mode Puzzle - Bonus de 25 points!", 320, 650, ui->couleurSucces);
            *score += 25;
            SDL_RenderPresent(renderer);
            SDL_Delay(1000);
            enCours = false;
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void afficherMenuPrincipal(SDL_Renderer *renderer, Interface *ui) {
    afficherBackground(renderer, ui);
    
    SDL_Rect titreRect = {200, 200, LARGEUR_ECRAN - 400, 100};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &titreRect);
    dessinerBordureSciFi(renderer, titreRect, 2, (SDL_Color){0, 255, 255, 255});
    
    afficherTexte(renderer, ui->fontTitre, "THE TOMORROW WAR", 350, 230, ui->couleurTexte);
    afficherTexte(renderer, ui->font, "Appuyez sur ESPACE pour commencer", 350, 400, ui->couleurTexte);
    afficherTexte(renderer, ui->font, "Appuyez sur M pour le menu Enigme", 360, 450, ui->couleurTexte);
    afficherTexte(renderer, ui->font, "Appuyez sur ESC pour quitter", 380, 500, ui->couleurTexte);
}
