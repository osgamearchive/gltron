#include "video/video.h"
#include "game/game.h"

#include "base/nebu_math.h"

/* draw a 2d map */

void draw2D( nebu_Rect *pRect ) {
	// TODO: modify for nebu_rect
	// get rid of the viewport requirement
	// assume rasonly is set, push/pop matrices
	int i;

	float grid_width = box2_Width(& game2->level->boundingBox);
	float grid_height = box2_Height(& game2->level->boundingBox);

	glPushMatrix();
	glTranslatef(pRect->x, pRect->y, 0);
	glScalef(pRect->width / grid_width, pRect->height / grid_height, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.7f, 0.7f, 0.7f, 0.3f);

	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	for(i = 0; i < game2->level->nBoundaries; i++) {
		vec2 v;
		segment2 *s = game2->level->boundaries + i;
		vec2_Add(&v, & s->vStart, & s->vDirection);
		glVertex2fv(s->vStart.v);
		glVertex2fv(v.v);
	}
	glEnd();

	for(i = 0; i < game->players; i++) {
		Player *p = &game->player[i];
		PlayerVisual *pV = gPlayerVisuals + i;
		segment2* trail;
		float x, y;

		getPositionFromData(&x, &y, p->data);
		
		// fixme: check if trails vanish
		if (p->data->trail_height <= 0) {
			continue;
		}

		if (p->data->trail_height < TRAIL_HEIGHT) {
			/* 
				if player crashed but the trail hasn't disappeared yet, fade
				the trail on the 2d map as it disappears.
			*/
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			glColor4f(pV->pColorAlpha[0], pV->pColorAlpha[1], pV->pColorAlpha[2],
					p->data->trail_height / TRAIL_HEIGHT);
		} else {
			glBlendFunc(GL_ONE, GL_ONE);
			glColor3fv(pV->pColorAlpha);
		}

		glPointSize(2);
		glBegin(GL_POINTS);
		glVertex2f( x, y );
		glEnd();

		glBegin(GL_LINES);
		for(trail = p->data->trails; trail != p->data->trails + p->data->trailOffset; trail++)
		{
			glVertex2f(trail->vStart.v[0], 
				trail->vStart.v[1]
				);
			glVertex2f(trail->vStart.v[0] + trail->vDirection.v[0], 
				trail->vStart.v[1] + trail->vDirection.v[1]
				);
		}
		if(trail != p->data->trails)
		{
			trail--;
			glVertex2f(trail->vStart.v[0] + trail->vDirection.v[0], 
				trail->vStart.v[1] + trail->vDirection.v[1]
				);
			glVertex2f( floorf(x), floorf(y));
		}
		else
		{
			glVertex2f(trail->vStart.v[0], 
				trail->vStart.v[1]
				);
			glVertex2f( floorf(x), floorf(y));
		}
			
	#if 0
		// draw AI debug lines
		glColor3f(1,1,1);
		glVertex2f(p->ai->front.vStart.v[0],
			p->ai->front.vStart.v[1]);
		glVertex2f(p->ai->front.vStart.v[0] + p->ai->front.vDirection.v[0],
			p->ai->front.vStart.v[1] + p->ai->front.vDirection.v[1]);
		glColor3f(0,1,0);
		glVertex2f(p->ai->left.vStart.v[0],
			p->ai->left.vStart.v[1]);
		glVertex2f(p->ai->left.vStart.v[0] + p->ai->left.vDirection.v[0],
			p->ai->left.vStart.v[1] + p->ai->left.vDirection.v[1]);
		glColor3f(0,0,1);
		glVertex2f(p->ai->right.vStart.v[0],
			p->ai->right.vStart.v[1]);
		glVertex2f(p->ai->right.vStart.v[0] + p->ai->right.vDirection.v[0],
			p->ai->right.vStart.v[1] + p->ai->right.vDirection.v[1]);
		glColor3f(0,1,1);
		glVertex2f(p->ai->backleft.vStart.v[0],
			p->ai->backleft.vStart.v[1]);
		glVertex2f(p->ai->backleft.vStart.v[0] + 
			p->ai->backleft.vDirection.v[0],
			p->ai->backleft.vStart.v[1] + 
			p->ai->backleft.vDirection.v[1]);
	#endif
		glEnd();
	}
	glDisable(GL_BLEND);

	glPopMatrix();
}
	
