#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include "display.h"
#include "player.h"
#include "game.h"
#include "enemies.h"
#include "shots.h"

void draw_player_shots(SHOT *shots)
{
    // desenha tiros do player
    SHOT *shot_aux;
    shot_aux = shots;

    while (shot_aux != NULL)
    {
        al_draw_filled_rectangle(shot_aux->x, shot_aux->y, shot_aux->x + 5, shot_aux->y + 20, WHITE);
        shot_aux = shot_aux->next;
    }
}

bool shot_in_this_column(SHOT *shots, PLAYER p)
{
    SHOT *aux;
    aux = shots;

    while (aux != NULL)
    {
        if (aux->x == p.x + SIZE_PLAYER + 5)
        {
            return TRUE;
        }
        aux = aux->next;
    }

    return FALSE;
}

/* SHOTS */
void delete_shot(int position, SHOT **head)
{
    SHOT *temp = *head;

    if (position == 0)
    {
        *head = (*head)->next;
        temp->next = NULL;
        free(temp);
    }
    else
    {
        for (int i = 0; i < position - 1; i++)
            temp = temp->next;

        SHOT *del = temp->next;
        temp->next = temp->next->next;
        del->next = NULL;
        free(del);
    }
}

/* Tiro colidiu com player */
bool player_collision(PLAYER player, ENEMY enemy)
{

    if (enemy.shots != NULL)
    {
        if ((enemy.shots->x <= (player.x + player.w)) &&
            (enemy.shots->x >= player.x) &&
            (enemy.shots->y <= (player.y + player.h)) &&
            (enemy.shots->y >= player.y))
            return TRUE; // colisão detectada
    }

    return FALSE; // sem colisão
}

void delete_enemy_shot(SHOT *head)
{
    SHOT *shot;
    shot = head;
    if (head == NULL || shot == NULL)
        return;

    head = head->next;
    free(shot);
    return;
}

/* Atualiza posição dos tiros inimigos */
void update_enemies_shots(ENEMY (*enemies)[ENEMIES_PER_LINE], PLAYER *player)
{
    int shooting_count = 0;

    for (int i = 0; i < NUM_ENEMIES_LINES; i++)
    {
        for (int j = 0; j < ENEMIES_PER_LINE; j++)
        {
            if (enemies[i][j].shots != NULL)
            {
                enemies[i][j].shots->y += enemies[i][j].height;
                // Adicione mensagens de depuração aqui
                // printf("Enemy Shot[%d][%d]: y=%f\n", i, j, enemies[i][j].shots->y);

                if (player_collision(*player, enemies[i][j]))
                {
                    player->lives--;
                    delete_enemy_shot(enemies[i][j].shots);
                }
                if (enemies[i][j].shots->y >= TOTAL_HEIGHT) // exclui o tiro
                {
                    enemies[i][j].shots = NULL;
                }
            }
        }
    }

    while (!enemy_active_shots(enemies, &shooting_count)) // Enquanto não tem 2 tiros acontecendo
    {
        int random_line = rand() % NUM_ENEMIES_LINES;
        int random_column = rand() % ENEMIES_PER_LINE;

        if (enemies[random_line][random_column].shots == NULL) // sem tiros ativos
        {
            enemies[random_line][random_column].shots = malloc(sizeof(SHOT));
            enemies[random_line][random_column].shots->x = enemies[random_line][random_column].x;
            enemies[random_line][random_column].shots->y = enemies[random_line][random_column].y;
            enemies[random_line][random_column].shots->direction = DOWN;
            shooting_count++;
        }
    }
}

int kill_enemy(ENEMY enemies[NUM_ENEMIES_LINES][ENEMIES_PER_LINE], SHOT *shot)
{
    for (int i = 0; i < NUM_ENEMIES_LINES; i++)
    {
        for (int j = 0; j < ENEMIES_PER_LINE; j++)
        {

            if ((enemies[i][j].state != DEAD_ENEMY) &&
                (shot->x >= enemies[i][j].x) &&
                (shot->x <= (enemies[i][j].x + enemies[i][j].width)) &&
                (shot->y >= enemies[i][j].y) &&
                (shot->y <= (enemies[i][j].y + enemies[i][j].height)))
            {
                enemies[i][j].state = DEAD_ENEMY;
                return TRUE;
            }
        }
    }

    return FALSE;
}

/* Retorna TRUE se houver 2 tiros ativos*/
int enemy_active_shots(ENEMY (*enemies)[ENEMIES_PER_LINE], int *active_shots)
{
    int shots = 0;
    for (int i = 0; i < NUM_ENEMIES_LINES; i++)
    {
        for (int j = 0; j < ENEMIES_PER_LINE; j++)
        {
            if (enemies[i][j].shots != NULL)
            {
                shots++;
            }
        }
    }

    *active_shots = shots;

    if (shots >= 2)
    {
        return TRUE;
    }

    return FALSE;
}

void update_player_shots(PLAYER *p, ENEMY (*enemies)[ENEMIES_PER_LINE])
{
    if (p->shots == NULL)
        return; // se não houver tiros, ignorar
    SHOT *aux;
    int i = -1;
    aux = p->shots;
    while (aux != NULL)
    {
        i++;
        aux->y -= SIZE_PLAYER / 2;

        if ((aux->y <= 0) | kill_enemy(enemies, aux))
        {
            delete_shot(i, &p->shots);
        }
        aux = aux->next;
    }
}

void create_player_shot(PLAYER *p)
{
    if (!shot_in_this_column(p->shots, *p))
    {
        SHOT *new;
        new = malloc(sizeof(SHOT));
        new->direction = UP;
        new->x = p->x + p->w / 2;
        new->y = p->y;
        new->next = p->shots;
        p->shots = new;
    }
}
