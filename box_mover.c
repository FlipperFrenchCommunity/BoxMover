/*
 * Project: BoxMover
 * File: box_mover.c
 * Create by: Rom1 <rom1@canel.ch> - Flipper French Community - https://github.com/FlipperFrenchCommunity
 * Date: 26 octobre 2022
 * License: GNU GENERAL PUBLIC LICENSE v3
 * Description: Programme d'apprentissage pour faire une application pour le
 *              Flipper Zero. Le programme récupère les entrées (boutons) et
 *              déplace un pixel sur l'affichage. Apprendre à générer un
 *              affichage et lire des entrées.
 */

/*
 * Importation des librairies
 */
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>


/*
 * Structure contenant le positionnement XY du pixel à déplacer.
 */
typedef struct {
    int x;
    int y;
} BoxMoverModel;

/*
 * Structure contenant l'état du programme.
 */
typedef struct {
    BoxMoverModel* model;          // Positionnement du pixel
    Gui* gui;                      // Gestion de l'affichage de l'écran (GUI)
    ViewPort* view_port;           // Etat de la vue de l'écran.
    FuriMessageQueue* event_queue; // Fil d'attente (Queue) pour passer des message (événements en entrée)
    FuriMutex* model_mutex;        // Verrou (Mutex) pour verouiller le fil d'exécution (Thread)
} BoxMoverState;

/**
 * Rappel de fonction pour dessiner l'écran.
 *
 * @param       Canvas pour l'affichage de l'écran
 * @param       Context contenant la variable d'état
 */
void box_mover_draw_callback(Canvas* canvas, void* context){
    /* Récupère la variable d'état du programme */
    BoxMoverState* box_mover = context;

    /* Aquérire le verrou  pour bloquer et modifier l'affichage sans risquei de
     * modification de box_mover->model */
    furi_check(furi_mutex_acquire(box_mover->model_mutex, 25) == FuriStatusOk);

    /* Modifier le canvas d'affichage */
    canvas_draw_box(canvas, box_mover->model->x, box_mover->model->y, 4, 4);

    /* Relacher le verrou */
    furi_mutex_release(box_mover->model_mutex);
}

/**
 * Rappel de fonction pour lire les entrée
 *
 * @param       Évenement en entrées
 * @param       Context contenant la variable d'état
 */
void box_mover_input_callback(InputEvent* input, void* context){
    /* Récupère la variable d'état du programme */
    BoxMoverState* box_mover = context;

    /* Aquérire le verrou  pour bloquer et modifier l'affichage sans risque */
    furi_check(furi_mutex_acquire(box_mover->model_mutex, 25) == FuriStatusOk);

    /* Met les événements d'entrée dans la fil d'attente */
    furi_message_queue_put(box_mover->event_queue, input, FuriWaitForever);

    /* Relacher le verrou */
    furi_mutex_release(box_mover->model_mutex);
}

/**
 * Fonction pour initialiser le programme. Elle va allouer les différentes ressources 
 * du programme dans la structure (BoxMoverState) contenant son état.
 *
 * @return      Structure contenant l'état du programme
 */
BoxMoverState* box_mover_alloc(){
    /* Alloue les variables pour la structure d'état et le positionnement du pixel */
    BoxMoverState* state = malloc(sizeof(BoxMoverState));
    state->model = malloc(sizeof(BoxMoverModel));
    state->model->x = 10;
    state->model->y = 10;

    /* Alloue la structure d'état pour view_port */
    state->view_port = view_port_alloc();
    /* Alloue le verrou (mutex) pour bloquer le fil d'exécution (thread) */
    state->model_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    /* Enregistre la ressource d'affichage */
    state->gui = furi_record_open(RECORD_GUI);
    /* Attache la view_port sur GUI (le gestionnaire d'affichage) */
    gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);

    /* Alloue une fil d'attente pour les événements en entrée */
    state->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    
    /* Définir les fonctions de rappel pour mettre à jour l'affichage ou la
     * saisie des entrées */
    view_port_draw_callback_set(state->view_port, box_mover_draw_callback, state);
    view_port_input_callback_set(state->view_port, box_mover_input_callback, state);
    
    return state; // Retourn la structure d'état
}

/**
 * Fonction pour terminer proprement le programme. Elle va libérer les ressource
 * avant de terminer le programme.
 *
 * @param       Structure contenant l'état du programme
 */
void box_mover_free(BoxMoverState* state){
    view_port_enabled_set(state->view_port, false);
    gui_remove_view_port(state->gui, state->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(state->view_port);

    furi_message_queue_free(state->event_queue);
    furi_mutex_free(state->model_mutex);

    free(state->model);
    free(state);
}

/**
 * Point d'entrée principal du programme.
 */
int32_t box_mover_app(void* p){
    UNUSED(p); // Bloque les erreurs de compilation d'une variable non-initialisée

    BoxMoverState* box_mover = box_mover_alloc(); // Créer l'état de notre programme


    InputEvent event; // Variable contenant les événements en entrée
    
    /* Boucle d'événement qui lit si une entrée saisie. La boucle est réalisée tant
     * que la touche retour en arrière n'est pas pressée. */
    for(bool processing=true ; processing ; ){

        /* Récupère les événement d'entrée mis dans la fil d'attente */
        FuriStatus event_status = furi_message_queue_get(box_mover->event_queue,
        &event, 100);

        /* Prend le verrou */
        furi_check(furi_mutex_acquire(box_mover->model_mutex, 25) == FuriStatusOk);

        /* Si il y a un événement faire */
        if(event_status == FuriStatusOk) {
            /* Si une entrée est pressée faire */
            if(event.type == InputTypePress) {
                /* Aiguille en fonction des divers entrées pressées */
                switch(event.key) {
                    /* Bouton du haut */
                    case InputKeyUp:
                        box_mover->model->y -= 2; // Déplace le pixel sur le haut
                        break;
                    /* Bouton du bas */
                    case InputKeyDown:
                        box_mover->model->y += 2; // Déplace le pixel sur le bas
                        break;
                    /* Bouton de gauche */
                    case InputKeyLeft:
                        box_mover->model->x -= 2; // Déplace le pixel sur la gauche
                        break;
                    /* Bouton de droite */
                    case InputKeyRight:
                        box_mover->model->x += 2; // Déplace le pixel sur la droite
                        break;

                    /* Bouton du centre et de retour en arrière */
                    case InputKeyOk:
                    case InputKeyBack:
                        processing = false; // Stop la boucle
                        break;
                }
            }
        }
        furi_mutex_release(box_mover->model_mutex); // Relache le verrou
        view_port_update(box_mover->view_port); // Met à jour l'affichage 
    }

    box_mover_free(box_mover); // Libère l'état de notre programme
    return 0;
}
