#include "game/game.h"
#include "game/ai.h"
#include <math.h>
#include <float.h>

#ifndef FLT_MAX
#define FLT_MAX 10000.0f
#endif

void ai_getClosestOpponent(int player, int* opponent, float *distance) {
	int i;
	vec2 v_player;
	vec2 v_opponent;

	*opponent = -1;
	*distance = FLT_MAX;

	getPositionFromIndex(v_player.v + 0, v_player.v + 1, player);
	
	for(i = 0; i < game->players; i++) {
		if(i == player)
			continue;
		if(game->player[i].data->speed > 0) {
			vec2 diff;
			float d;

			getPositionFromIndex(v_opponent.v + 0, v_opponent.v + 1, i);
			vec2Sub(&diff, &v_player, &v_opponent);
			// use manhattan distance instead of euclidean distance
			d = fabs(diff.v[0]) + fabs(diff.v[1]);
			// d = vec2Length(&diff);
			if(d < *distance) {
				*opponent = i;
				*distance = d;
			}
		}
	}
} 

void ai_getDistances(int player, AI_Distances *distances) {
	enum { eFront = 0, eLeft, eRight, eBackleft, eMax };
	segment2 segments[eMax];
	vec2 v, vPos;
	Data *data = game->player[player].data;
	int dirLeft = (data->dir + 3) % 4;
	int dirRight = (data->dir + 1) % 4;
	int i, j;
	float *front = &distances->front;
	float *right = &distances->right;
	float *left = &distances->left;
	float *backleft = &distances->backleft;

	getPositionFromIndex(vPos.v + 0, vPos.v + 1, player);

	for(i = 0; i < eMax; i++) {
		vec2Copy(&segments[i].vStart, &vPos);
	}

	segments[eFront].vDirection.v[0] = dirsX[data->dir];
	segments[eFront].vDirection.v[1] = dirsY[data->dir];
	segments[eLeft].vDirection.v[0] = dirsX[dirLeft];
	segments[eLeft].vDirection.v[1] = dirsY[dirLeft];
	segments[eRight].vDirection.v[0] = dirsX[dirRight];
	segments[eRight].vDirection.v[1] = dirsY[dirRight];
	segments[eBackleft].vDirection.v[0] = dirsX[dirLeft] - dirsX[data->dir];
	segments[eBackleft].vDirection.v[1] = dirsY[dirLeft] - dirsY[data->dir];
	vec2Normalize(&segments[eBackleft].vDirection,
								&segments[eBackleft].vDirection);
	*front = FLT_MAX;
	*left = FLT_MAX;
	*right = FLT_MAX;
	*backleft = FLT_MAX;

	// loop over all segment
	for(i = 0; i < game->players; i++) {
		segment2 *wall = game->player[i].data->trails;
		if(game->player[i].data->trail_height < TRAIL_HEIGHT)
			continue;

		for(j = 0; j < game->player[i].data->trailOffset + 1; j++) {
			float t1, t2;
			if(i == player && j == game->player[i].data->trailOffset)
				break;
			if(segment2_Intersect(&v, &t1, &t2, segments + eFront, wall) &&
				 t1 > 0 && t1 < *front && t2 >= 0 && t2 <= 1)
				*front = t1;
			if(segment2_Intersect(&v, &t1, &t2, segments + eLeft, wall) &&
				 t1 > 0 && t1 < *left && t2 >= 0 && t2 <= 1)
				*left = t1;
			if(segment2_Intersect(&v, &t1, &t2, segments + eRight, wall) &&
				 t1 > 0 && t1 < *right && t2 >= 0 && t2 <= 1)
				*right = t1;
			if(segment2_Intersect(&v, &t1, &t2, segments + eBackleft, wall) &&
				 t1 > 0 && t1 < *backleft && t2 >= 0 && t2 <= 1)
				*backleft = t1;
			wall++;
		}
	}
	for(i = 0; i < 4; i++) {
		float t1, t2;
		segment2* wall = walls + i;
		if(segment2_Intersect(&v, &t1, &t2, segments + eFront, wall) &&
			 t1 > 0 && t1 < *front && t2 >= 0 && t2 <= 1)
			*front = t1;
		if(segment2_Intersect(&v, &t1, &t2, segments + eLeft, wall) &&
			 t1 > 0 && t1 < *left && t2 >= 0 && t2 <= 1)
			*left = t1;
		if(segment2_Intersect(&v, &t1, &t2, segments + eRight, wall) &&
			 t1 > 0 && t1 < *right && t2 >= 0 && t2 <= 1)
			*right = t1;
		if(segment2_Intersect(&v, &t1, &t2, segments + eBackleft, wall) &&
			 t1 > 0 && t1 < *backleft && t2 >= 0 && t2 <= 1)
			*backleft = t1;
	}
	
	// update debug render segments
	{
		AI *ai = game->player[player].ai;
		vec2Copy(&ai->front.vStart, &vPos);
		vec2Copy(&ai->left.vStart, &vPos);
		vec2Copy(&ai->right.vStart, &vPos);
		vec2Copy(&ai->backleft.vStart, &vPos);
		
		ai->front.vDirection.v[0] = *front * dirsX[data->dir];
		ai->front.vDirection.v[1] = *front * dirsY[data->dir];
		ai->left.vDirection.v[0] = *left * dirsX[dirLeft];
		ai->left.vDirection.v[1] = *left * dirsY[dirLeft];
		ai->right.vDirection.v[0] = *right * dirsX[dirRight];
		ai->right.vDirection.v[1] = *right * dirsY[dirRight];
		ai->backleft.vDirection.v[0] = dirsX[dirLeft] - dirsX[data->dir];
		ai->backleft.vDirection.v[1] = dirsY[dirLeft] - dirsY[data->dir];
		vec2Normalize(&ai->backleft.vDirection,
									&ai->backleft.vDirection);
		vec2Scale(&ai->backleft.vDirection, 
							&ai->backleft.vDirection,
							*backleft);
	}
		
	// printf("%.2f, %.2f, %.2f\n", *front, *right, *left);
	return;
}

void ai_getConfig(int player, int target,
										AI_Configuration *config) {

	Data *data;
	
	getPositionFromIndex(config->player.vStart.v + 0,
											 config->player.vStart.v + 1,
											 player);
	getPositionFromIndex(config->opponent.vStart.v + 0,
											 config->opponent.vStart.v + 1,
											 target);
	
	data = game->player[player].data;
	config->player.vDirection.v[0] = dirsX[ data->dir ] * data->speed;
	config->player.vDirection.v[1] = dirsY[ data->dir ] * data->speed;

	data = game->player[target].data;
	config->opponent.vDirection.v[0] = dirsX[ data->dir ] * data->speed;
	config->opponent.vDirection.v[1] = dirsY[ data->dir ] * data->speed;
	
	// compute sector
	{
		vec2 diff;
		vec3 v1, v2, v3;
		vec3 up = { { 0, 0, 1 } };
		float cosphi;
		float phi;
		int i;

		vec2Sub(&diff, &config->player.vStart, &config->opponent.vStart);
		v1.v[0] = diff.v[0];
		v1.v[1] = diff.v[1];
		v1.v[2] = 0;

		v2.v[0] = config->opponent.vDirection.v[0];
		v2.v[1] = config->opponent.vDirection.v[1];
		v2.v[2] = 0;

		vec3Normalize(&v1, &v1);
		vec3Normalize(&v2, &v2);

		vec3Cross(&v3, &v1, &v2);
		vec3Normalize(&v3, &v3);
	
		cosphi = vec3Dot(&v1, &v2);
		clamp(&cosphi, -1, 1);
		phi = acos(cosphi);
		if(vec3Dot(&v3, &up) > 0)
			phi = 2 * M_PI - phi;
	
		for(i = 0; i < 8; i++) {
			phi -= M_PI / 4;
			if(phi < 0) {
				config->location = i;
				break;
			}
		}
	}
	// compute intersection
	{
		segment2 seg1;
		segment2 seg2;
		seg1.vStart = config->opponent.vStart;
		seg1.vDirection = config->opponent.vDirection;
		seg2.vStart = config->player.vStart;
		vec2_Orthogonal( &seg2.vDirection, &config->opponent.vDirection );
		vec2Normalize( &seg2.vDirection, &seg2.vDirection );
		vec2Scale( &seg2.vDirection, 
							 &seg2.vDirection,
							 vec2Length( &config->player.vDirection )
							 );
							 
		segment2_Intersect( &config->intersection, 
												&config->t_opponent, &config->t_player,
												&seg1, &seg2 );
		if(config->t_player < 0)
			config->t_player *= -1;
	}
	
}

void ai_left(int player, AI_Distances *distances) {
	// printf("trying left turn...");
	AI *ai = game->player[player].ai;
	Data *data = game->player[player].data;
	int level = gSettingsCache.ai_level;

	float save_distance = 
		(ai_params.minTurnTime[level] * data->speed / 1000.0f) + 20;
	
	if(distances->left > save_distance) {
		createEvent(player, EVENT_TURN_LEFT);
		ai->tdiff++;
		ai->lasttime = game2->time.current;
		// printf("succeeded\n");
	} else {
		// printf("failed\n");
	}
}

void ai_right(int player, AI_Distances *distances) {
	// printf("trying right turn...");
	AI *ai = game->player[player].ai;
	Data *data = game->player[player].data;
	int level = gSettingsCache.ai_level;

	float save_distance = 
		(ai_params.minTurnTime[level] * data->speed / 1000.0f) + 20;
	
	if(distances->right > save_distance) {
		createEvent(player, EVENT_TURN_RIGHT);
		ai->tdiff--;
		ai->lasttime = game2->time.current;
		// printf("succeeded\n");
	} else {
		// printf("failed\n");
	}
}

int agressive_action[8][4] = {
	{ 2, 0, 2, 2 },
	{ 0, 1, 1, 2 },
	{ 0, 1, 1, 2 },
	{ 0, 1, 1, 2 },
	{ 0, 2, 2, 1 },
	{ 0, 2, 2, 1 },
	{ 0, 2, 2, 1 },
	{ 1, 1, 1, 0 }
};

int evasive_action[8][4] = {
	{ 1, 1, 2, 2 },
	{ 1, 1, 2, 0 },
	{ 1, 1, 2, 0 },
	{ 1, 1, 2, 0 },
	{ 2, 0, 1, 1 },
	{ 2, 0, 1, 1 },
	{ 2, 0, 1, 1 },
	{ 2, 2, 1, 1 }
};


void ai_action(int action, int player, AI_Distances *distances) {
	switch(action) {
	case 0: break;
	case 1: ai_left(player, distances); break;
	case 2: ai_right(player, distances); break;
	}
}
		
void ai_aggressive(int player, int target, int location,
									 AI_Distances *distances) {
	int dirdiff = (4 + 
								 game->player[player].data->dir -
								 game->player[target].data->dir) % 4;
	
	// printf("aggressive mode (%d, %d)\n", player, target, location, dirdiff);

	ai_action(agressive_action[location][dirdiff], player, distances);
}

void ai_evasive(int player, int target, int location, 
								AI_Distances *distances) {
	int dirdiff = (4 + 
								 game->player[player].data->dir -
								 game->player[target].data->dir) % 4;
	// printf("evasive mode (%d,%d,%d)\n", player, target, location);
	
	ai_action(evasive_action[location][dirdiff], player, distances);
}

