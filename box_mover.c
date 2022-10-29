/*
 * Project: BoxMover_PoC
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
    BoxMoverModel* model; // Positionnement du pixel
    Gui* gui; // Gestion de l'affichage de l'écran (GUI)
    ViewPort* view_port; // Etat de la vue de l'écran.
    FuriMessageQueue* event_queue; // Fil d'attente (Queue) pour passer des message
    FuriMutex* model_mutex; // Verrou (Mutex) pour verouiller le fil d'exécution (Thread)
} BoxMoverState;

/*
 * Rappel de fonction pour dessiner l'écran.
 */
void box_mover_draw_callback(Canvas* canvas, void* context){
    BoxMoverState* box_mover = context;
    furi_check(furi_mutex_acquire(box_mover->model_mutex, 25) == FuriStatusOk);
    canvas_draw_box(canvas, box_mover->model->x, box_mover->model->y, 4, 4);
    furi_mutex_release(box_mover->model_mutex);
}

/*
 * Rappel de fonction pour lire les entrée
 */
void box_mover_input_callback(InputEvent* input, void* context){
    BoxMoverState* box_mover = context;
    furi_check(furi_mutex_acquire(box_mover->model_mutex, 25) == FuriStatusOk);
    furi_message_queue_put(box_mover->event_queue, input, FuriWaitForever);
    furi_mutex_release(box_mover->model_mutex);
}

/*
 * Fonction pour initialiser le programme. Elle va allouer les différentes ressources 
 * du programme dans la structure (BoxMoverState) contenant son état.
 */
BoxMoverState* box_mover_alloc(){
    BoxMoverState* state = malloc(sizeof(BoxMoverState));
    state->model = malloc(sizeof(BoxMoverModel));
    state->model->x = 10;
    state->model->y = 10;

    state->view_port = view_port_alloc();
    state->model_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    state->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);

    state->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    
    /* 
     * Définir les fonctions de rappel pour mettre à jour l'affichage ou la
     * saisie des entrées
     */
    view_port_draw_callback_set(state->view_port, box_mover_draw_callback, state);
    view_port_input_callback_set(state->view_port, box_mover_input_callback, state);
    
    return state;
}

/*
 * Fonction pour terminer proprement le programme. Elle va libérer les ressource
 * avant de terminer le programme.
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

/*
 * Point d'entrée principal du programme.
 */
int32_t box_mover_app(void* p){
    UNUSED(p); // Bloque les erreurs de compilation d'une variable non-initialisée

    /*
     * Créer l'état de notre programme.
     */
    BoxMoverState* box_mover = box_mover_alloc();

    /*
     * Boucle d'événement qui recherche si il y a une entrée.
     */
    InputEvent event;
    for(bool processing=true ; processing ; ){
        FuriStatus event_status = furi_message_queue_get(box_mover->event_queue,
        &event, 100);

        furi_check(furi_mutex_acquire(box_mover->model_mutex, 25) == FuriStatusOk);

        if(event_status == FuriStatusOk) {
            if(event.type == InputTypePress) {
                switch(event.key) {
                    case InputKeyUp:
                        box_mover->model->y -= 2;
                        break;
                    case InputKeyDown:
                        box_mover->model->y += 2;
                        break;
                    case InputKeyLeft:
                        box_mover->model->x -= 2;
                        break;
                    case InputKeyRight:
                        box_mover->model->x += 2;
                        break;
                    case InputKeyOk:
                    case InputKeyBack:
                        processing = false;
                        break;
                }
            }
        }
        furi_mutex_release(box_mover->model_mutex);
        view_port_update(box_mover->view_port);
    }

    box_mover_free(box_mover);
    return 0;
}
