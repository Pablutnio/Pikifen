#include "gui.h"

/*
 * Creates a gui.
 * autonomous: if true, the gui will work without there being a need to create a display, feed it events, etc.
 * display: if the gui is not autonomous, this is the display it should be drawn on.
 * close_button_quits: if the gui is autonomous, this specifies whether the close button on the window makes the application stop or not.
 * display_w: if the gui is autonomous, it'll create a display with this width.
 * display_h: if the gui is autonomous, it'll create a display with this height.
 * style: the widget style.
 * flags: widget flags. Use LAFI_FLAG_*.
 */
lafi_gui::lafi_gui(int w, int h, lafi_style* style, unsigned char flags)
    : lafi_widget(0, 0, w, h, style, flags) {
    
    this->autonomous = false;
}

/*lafi_gui::lafi_gui(unsigned int display_w, unsigned int display_h, bool close_button_quits, lafi_style* style, unsigned char flags)
    : lafi_widget(0, 0, display_w, display_h, style, flags) {

    autonomous = true;
    this->close_button_quits = close_button_quits;

    display = NULL;
    timer = NULL;

    al_init();
    al_install_mouse();
    al_install_keyboard();
    al_init_image_addon();
    al_init_font_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_mouse();

    display = al_create_display(display_w, display_h);

    timer = al_create_timer(1.0 / 30.0);

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    thread = al_create_thread(thread_code, (void*) this);
    al_start_thread(thread);
}*/

void* lafi_gui::thread_code(ALLEGRO_THREAD*, void* gui) {
    lafi_gui* gui_ptr = (lafi_gui*) gui;
    
    al_start_timer(gui_ptr->timer);
    
    while(1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(gui_ptr->queue, &ev);
        gui_ptr->handle_event(ev);
        
        if(ev.type == ALLEGRO_EVENT_TIMER && al_is_event_queue_empty(gui_ptr->queue)) {
            gui_ptr->draw();
            al_flip_display();
        } else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return NULL;
        }
    }
    
    return NULL;
}

void lafi_gui::stop() {
    if(!autonomous) return;
    
    al_destroy_thread(thread);
}

void lafi_gui::wait() {
    al_join_thread(thread, NULL);
}

//Destroys a gui.
lafi_gui::~lafi_gui() { }

void lafi_gui::draw_self() {
    al_draw_filled_rectangle(x1, y1, x2, y2, style->bg_color);
}

void lafi_gui::render() { }