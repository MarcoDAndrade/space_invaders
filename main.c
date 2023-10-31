#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define KEY_SEEN 1
#define KEY_RELEASED 2

#define TRUE 1
#define FALSE 0

#define HEIGHT_DISPLAY 800
#define WIDTH_DISPLAY 1000
#define MARGIN 100
#define TOTAL_WIDTH WIDTH_DISPLAY + MARGIN
#define TOTAL_HEIGHT HEIGHT_DISPLAY + MARGIN

#define SIZE_PLAYER 50

typedef struct shot
{

    struct shot *next;
    int direction;

} SHOT;

typedef struct player
{
    float x, y;
    int score;
} PLAYER;

typedef struct enemy
{
    float x, y;
    int size, qtdd;
    struct enemy *next;
    int r, g, b; // cor do inimigo
} ENEMY;

void must_init(bool test, const char *description)
{
    if (test)
        return;

    printf("Couldn't initialize %s\n", description);
    exit(1);
}

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY *disp = al_create_display(TOTAL_WIDTH, TOTAL_HEIGHT);
    must_init(disp, "display");

    ALLEGRO_FONT *font = al_create_builtin_font();
    must_init(font, "font");
    must_init(al_init_primitives_addon(), "primitives");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;

    ALLEGRO_EVENT event;

    PLAYER player;
    player.x = WIDTH_DISPLAY / 2;
    player.y = HEIGHT_DISPLAY - SIZE_PLAYER;
    player.score = 0;

    ENEMY *enemy;
    enemy = malloc(sizeof(ENEMY));
    enemy->size = SIZE_PLAYER;
    enemy->qtdd = 1;
    enemy->next = NULL;
    enemy->x = 0;
    enemy->y = 100 - SIZE_PLAYER;

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    al_start_timer(timer);

    while (1)
    {
        al_wait_for_event(queue, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:

            if (key[ALLEGRO_KEY_LEFT] && (player.x != MARGIN))
            {
                player.x -= SIZE_PLAYER / 2;
            }
            else if (key[ALLEGRO_KEY_RIGHT] && player.x != TOTAL_WIDTH - SIZE_PLAYER - MARGIN)
            {
                player.x += SIZE_PLAYER / 2;
            }
            else if (key[ALLEGRO_KEY_ESCAPE])
                done = true;

            else if (key[ALLEGRO_KEY_SPACE])
            {
                done = true;
            }

            for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
                key[i] &= KEY_SEEN;

            redraw = true;
            break;

        case ALLEGRO_EVENT_KEY_DOWN:                               // tecla pressionada
            key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED; // armazena no vetor na posição respectiva ao keycode da tecla
            break;

        case ALLEGRO_EVENT_KEY_UP: // tecla liberada
            key[event.keyboard.keycode] &= KEY_RELEASED;
            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        if (done)
            break;

        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 0, 0, "X: %.1f Y: %.1f", player.x, player.y);
            al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 50, 0, "SCORE: %d", player.score);

            al_draw_filled_rectangle(enemy->x, enemy->y, enemy->x + SIZE_PLAYER, enemy->y + SIZE_PLAYER, al_map_rgb(255, 0, 0));

            al_draw_filled_rectangle(player.x, player.y, player.x + SIZE_PLAYER, player.y + SIZE_PLAYER, al_map_rgb(255, 255, 255));

            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}