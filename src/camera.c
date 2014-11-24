#include <stdlib.h>

#include "camera.h"

void camera_fill_rect(camera_t *camera, rect_t *rect, int32_t color);
void camera_draw_terrain_rects(camera_t *camera, game_t *game);
void camera_draw_platform_rects(camera_t *camera, game_t *game);
void camera_draw_player(camera_t *camera, player_t *player);
void camera_draw_enemies(camera_t *camera, game_t *game);
void camera_draw_targets(camera_t *camera, game_t *game);
void camera_draw_debug_info(camera_t *camera, game_t *game);

camera_t *camera_create(void){
	camera_t *camera = malloc(sizeof(camera_t));
	camera->view = rect_create();
	camera->bounds = rect_create();
	camera->buffer = NULL;
	
	return camera;
}

void camera_init(camera_t *camera, int32_t w, int32_t h){
	camera->view->w = w;
	camera->view->h = h;
	camera->buffer = create_surface(w, h);
}

void camera_delete(camera_t *camera){
	rect_delete(camera->view);
	SDL_FreeSurface(camera->buffer);
}

void camera_fill_rect(camera_t *camera, rect_t *rect, int32_t color){
	SDL_Rect fill_rect;

	rect_copy_to_sdl(rect, &fill_rect);
	
	fill_rect.x -= (int)camera->view->x;
	fill_rect.y -= (int)camera->view->y;

	SDL_FillRect(camera->buffer, &fill_rect, color);
}

void camera_draw_sprite(camera_t *camera, sprite_t *sprite){
	SDL_Rect draw_rect;

	rect_copy_to_sdl(sprite->rect, &draw_rect);
	
	draw_rect.x -= (int)camera->view->x;
	draw_rect.y -= (int)camera->view->y;

	anim_draw(sprite->anim, sprite->step, camera->buffer, &draw_rect);
}

void camera_draw_surface(camera_t *camera, SDL_Surface *surface){
	SDL_Rect draw_rect;

	draw_rect.x = 0 - (int)camera->view->x;
	draw_rect.y = 0 - (int)camera->view->y;
	draw_rect.w = surface->w;
	draw_rect.h = surface->h;

	SDL_BlitSurface(surface, NULL, camera->buffer, &draw_rect);
}

void camera_draw_game(camera_t *camera, game_t *game){
	camera->view->x = floor(camera->view->x);
	camera->view->y = floor(camera->view->y);

	SDL_FillRect(camera->buffer, NULL, 0xDDDDDDFF);

	rect_limit_to(camera->view, camera->bounds);

	#ifdef DEBUG
	camera_draw_terrain_rects(camera, game);
	camera_draw_platform_rects(camera, game);
	#endif
	
	camera_draw_surface(camera, game->map_image);
	camera_draw_targets(camera, game);
	camera_draw_enemies(camera, game);

	camera_draw_player(camera, game->player);

	#ifdef DEBUG
	camera_draw_debug_info(camera, game);
	#endif
}

void camera_draw_terrain_rects(camera_t *camera, game_t *game){
	rect_node_t *iter = game->terrain_rects->head;
	while(iter != NULL){
		camera_fill_rect(camera, iter->data, 0x333366FF);
		iter = iter->next;
	}
}

void camera_draw_platform_rects(camera_t *camera, game_t *game){
	rect_node_t *iter = game->platform_rects->head;
	while(iter != NULL){
		camera_fill_rect(camera, iter->data, 0x7777AAFF);
		iter = iter->next;
	}
}

void camera_draw_player(camera_t *camera, player_t *player){
	
	if(player->flashing % 2 == 0){
		#ifdef DEBUG
		//camera_fill_rect(camera, player->body->rect, 0x2222DDFF);
		#endif
		
		camera_draw_sprite(camera, player->sprite);
	}
	
	camera_fill_rect(camera, player->weapon, 0xDD9900FF);
}

void camera_draw_targets(camera_t *camera, game_t *game)
{
	target_node_t *iter = game->targets->head;
	while(iter != NULL)
	{
		#ifdef DEBUG
		#endif

		if(iter->data->sprite != NULL){
			camera_draw_sprite(camera, iter->data->sprite);
		}
		
		iter = iter->next;
	}
}

void camera_draw_enemies(camera_t *camera, game_t *game)
{
	enemy_node_t *iter = game->enemies->head;
	while(iter != NULL)
	{
		if(iter->data->alive){
			if(iter->data->flashing % 2 == 0){
				camera_fill_rect(camera, iter->data->rect, 0xFF0000FF);
			}
		}
		iter = iter->next;
	}
}


void camera_draw_debug_info(camera_t *camera, game_t *game){
	char buffer[128] = "\0";

	int line_no = 0;
	int line_h = 14;

	#define PRINT_DEBUG_LINE font_draw_string(game->font, buffer, 4, 2 + (line_no * line_h), camera->buffer); line_no++;

	sprintf(buffer, "Win Size: %ix%i", game->core->win_cw, game->core->win_ch);
	PRINT_DEBUG_LINE

	sprintf(buffer, "Scale: %04.2f", core_get_scale(game->core));
	PRINT_DEBUG_LINE

	sprintf(buffer, "Time: %06.2fs", ((double)game->step)/60.0);
	PRINT_DEBUG_LINE

	sprintf(buffer, "Player Pos: %4.0f,%4.0f", game->player->body->rect->x, game->player->body->rect->y);
	PRINT_DEBUG_LINE

	#undef PRINT_DEBUG_LINE
}



