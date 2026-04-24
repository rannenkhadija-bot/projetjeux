// main.c
#include "fichier.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        return 1;
    }
    
    if (TTF_Init() < 0) {
        return 1;
    }
    
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        return 1;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        return 1;
    }
    
    Mix_Music* music = Mix_LoadMUS("assets/sound_back.mp3");
    if (music) Mix_PlayMusic(music, -1);
    
    SDL_Window *window = SDL_CreateWindow("LOT 5",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          LARGEUR_ECRAN, HAUTEUR_ECRAN,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        return 1;
    }
    
    Interface ui;
    initInterface(renderer, &ui);
    
    Question questions[MAX_QUESTIONS];
    int nbQuestions = 0;
    chargerQuestions("questions.txt", questions, &nbQuestions, renderer);
    
    if (nbQuestions == 0) {
        return 1;
    }
    
    int score = 0;
    int vies = 3;
    int level = 1;
    int questionsRepondues = 0;
    float tempsRestant = TEMPS_PAR_QUESTION;
    Uint32 dernierTemps = SDL_GetTicks();
    int jeuActif = 0;
    int menuActif = 1;
    int questionActuelle = -1;
    
    Bouton boutons[4];
    int sourisX, sourisY;
    
    srand(time(NULL));
    
    SDL_Event event;
    int running = 1;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
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
                    int succes = (reponse == bonneReponse);
                    
                    if (succes) {
                        verifierReponse(bonneReponse, reponse, &score, &vies, &level, &questionsRepondues);
                        afficherMessageRotozoom(renderer, &ui, "BONNE REPONSE!", 1);
                    } else {
                        vies--;
                        afficherMessageRotozoom(renderer, &ui, "MAUVAISE REPONSE!", 0);
                    }
                    
                    if (vies <= 0) {
                        jeuActif = 0;
                        menuActif = 1;
                        afficherMessageRotozoom(renderer, &ui, "GAME OVER!", 0);
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
                            jeuActif = 0;
                            menuActif = 1;
                            afficherMessageRotozoom(renderer, &ui, "VICTOIRE!", 1);
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
                        jeuActif = 1;
                        menuActif = 0;
                        questionActuelle = choisirQuestion(questions, nbQuestions);
                        if (questionActuelle >= 0) {
                            questions[questionActuelle].dejaVu = 1;
                            tempsRestant = TEMPS_PAR_QUESTION;
                            initialiserBoutons(boutons, questions[questionActuelle]);
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = 0;
                    }
                }
                else if (jeuActif && event.key.keysym.sym == SDLK_ESCAPE) {
                    jeuActif = 0;
                    menuActif = 1;
                }
            }
        }
        
        Uint32 tempsActuel = SDL_GetTicks();
        float deltaTime = (tempsActuel - dernierTemps) / 1000.0f;
        dernierTemps = tempsActuel;
        
        if (jeuActif && questionActuelle >= 0) {
            miseAJourTemps(&tempsRestant, deltaTime, &vies);
            
            if (tempsRestant <= 0) {
                afficherMessageRotozoom(renderer, &ui, "TEMPS ECOULE!", 0);
                vies--;
                
                if (vies <= 0) {
                    jeuActif = 0;
                    menuActif = 1;
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
            SDL_Texture* bgTex = questions[questionActuelle].backgroundTex;
            afficherBackground(renderer, &ui, bgTex);
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
    
    if (music) Mix_FreeMusic(music);
    Mix_CloseAudio();
    cleanupInterface(&ui);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
