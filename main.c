#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "fonction.h"

int main(int argc, char *argv[]) {
    // Initialisation SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    if (TTF_Init() < 0) {
        printf("Erreur TTF: %s\n", TTF_GetError());
        return 1;
    }
    
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        printf("Erreur IMG: %s\n", IMG_GetError());
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erreur Mixer: %s\n", Mix_GetError());
    }
    
    SDL_Window *window = SDL_CreateWindow("The Tomorrow War - Le Jeu",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          LARGEUR_ECRAN, HAUTEUR_ECRAN,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur fenêtre: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erreur renderer: %s\n", SDL_GetError());
        return 1;
    }
    
    Interface ui;
    initInterface(renderer, &ui);
    
    Question questions[MAX_QUESTIONS];
    int nbQuestions = 0;
    chargerQuestions("questions.txt", questions, &nbQuestions);
    
    if (nbQuestions == 0) {
        printf("Aucune question chargée!\n");
        return 1;
    }
    
    int score = 0;
    int vies = 3;
    int level = 1;
    int questionsRepondues = 0;
    float tempsRestant = TEMPS_PAR_QUESTION;
    Uint32 dernierTemps = SDL_GetTicks();
    bool jeuActif = false;
    bool menuActif = true;
    int questionActuelle = -1;
    
    Bouton boutons[4];
    int sourisX, sourisY;
    
    srand(time(NULL));
    
    SDL_Event event;
    bool running = true;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEMOTION) {
                sourisX = event.motion.x;
                sourisY = event.motion.y;
                
                if (jeuActif) {
                    for (int i = 0; i < 4; i++) {
                        boutons[i].hover = (sourisX >= boutons[i].rect.x && sourisX <= boutons[i].rect.x + boutons[i].rect.w &&
                                            sourisY >= boutons[i].rect.y && sourisY <= boutons[i].rect.y + boutons[i].rect.h);
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && jeuActif) {
                int reponse = gererClicBoutons(boutons, 4, event.button.x, event.button.y);
                if (reponse >= 0) {
                    int bonneReponse = questions[questionActuelle].bonneReponse;
                    bool succes = (reponse == bonneReponse);
                    
                    if (succes) {
                        verifierReponse(bonneReponse, reponse, &score, &vies, &level, &questionsRepondues);
                        afficherMessageRotozoom(renderer, &ui, "BONNE REPONSE!", true);
                    } else {
                        vies--;
                        afficherMessageRotozoom(renderer, &ui, "MAUVAISE REPONSE!", false);
                    }
                    
                    if (vies <= 0) {
                        jeuActif = false;
                        menuActif = true;
                        afficherMessageRotozoom(renderer, &ui, "GAME OVER!", false);
                        SDL_Delay(2000);
                        score = 0;
                        vies = 3;
                        level = 1;
                        questionsRepondues = 0;
                        for (int i = 0; i < nbQuestions; i++) {
                            questions[i].dejaVu = 0;
                        }
                    } else {
                        questionActuelle = choisirQuestion(questions, nbQuestions);
                        if (questionActuelle >= 0) {
                            questions[questionActuelle].dejaVu = 1;
                            tempsRestant = TEMPS_PAR_QUESTION;
                            initialiserBoutons(boutons, questions[questionActuelle]);
                        } else {
                            jeuActif = false;
                            menuActif = true;
                            afficherMessageRotozoom(renderer, &ui, "VICTOIRE!", true);
                            SDL_Delay(2000);
                            for (int i = 0; i < nbQuestions; i++) {
                                questions[i].dejaVu = 0;
                            }
                            score = 0;
                            vies = 3;
                            level = 1;
                            questionsRepondues = 0;
                        }
                    }
                }
            }
            else if (event.type == SDL_KEYDOWN) {
                if (menuActif) {
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        jeuActif = true;
                        menuActif = false;
                        questionActuelle = choisirQuestion(questions, nbQuestions);
                        if (questionActuelle >= 0) {
                            questions[questionActuelle].dejaVu = 1;
                            tempsRestant = TEMPS_PAR_QUESTION;
                            initialiserBoutons(boutons, questions[questionActuelle]);
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_m) {
                        sousMenuEnigme(renderer, &ui, questions, nbQuestions, &score, &vies, level);
                    }
                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                }
                else if (jeuActif && event.key.keysym.sym == SDLK_ESCAPE) {
                    jeuActif = false;
                    menuActif = true;
                }
                else if (jeuActif && event.key.keysym.sym == SDLK_m) {
                    sousMenuEnigme(renderer, &ui, questions, nbQuestions, &score, &vies, level);
                }
            }
        }
        
        Uint32 tempsActuel = SDL_GetTicks();
        float deltaTime = (tempsActuel - dernierTemps) / 1000.0f;
        dernierTemps = tempsActuel;
        
        if (jeuActif && questionActuelle >= 0) {
            miseAJourTemps(&tempsRestant, deltaTime, &vies);
            
            if (tempsRestant <= 0) {
                afficherMessageRotozoom(renderer, &ui, "TEMPS ECOULE!", false);
                vies--;
                
                if (vies <= 0) {
                    jeuActif = false;
                    menuActif = true;
                    SDL_Delay(2000);
                    score = 0;
                    vies = 3;
                    level = 1;
                    questionsRepondues = 0;
                    for (int i = 0; i < nbQuestions; i++) {
                        questions[i].dejaVu = 0;
                    }
                } else {
                    questionActuelle = choisirQuestion(questions, nbQuestions);
                    if (questionActuelle >= 0) {
                        questions[questionActuelle].dejaVu = 1;
                        tempsRestant = TEMPS_PAR_QUESTION;
                        initialiserBoutons(boutons, questions[questionActuelle]);
                    }
                }
            }
        }
        
        SDL_RenderClear(renderer);
        
        if (menuActif) {
            afficherMenuPrincipal(renderer, &ui);
        }
        else if (jeuActif && questionActuelle >= 0) {
            afficherBackground(renderer, &ui);
            afficherScoreVies(renderer, &ui, score, vies, level);
            afficherQuestion(renderer, &ui, questions[questionActuelle]);
            afficherBoutonsReponse(renderer, &ui, boutons, 4);
            afficherChronometreBarre(renderer, 150, HAUTEUR_ECRAN - 100, LARGEUR_ECRAN - 300, 30, 
                                      tempsRestant / TEMPS_PAR_QUESTION);
        }
        
        if (ui.animation.actif) {
            afficherMessageRotozoom(renderer, &ui, ui.animation.message, ui.animation.succes);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    cleanupInterface(renderer, &ui);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
