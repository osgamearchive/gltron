#include "video/video.h"
#include "game/game.h"

/* draw a 2d map */

void draw2D( Visual *d ) {
		float width, height;
		float border_left, border_bottom;
		int i;

		float aspect = (float)d->vp_w / (float)d->vp_h;
		if(d->vp_w / game2->grid.width < d->vp_h / game2->grid.height) {
				// black borders top/bottom
				width = game2->grid.width + 1.0f;
				height = width / aspect;
				border_bottom = (height - game2->grid.height) / 2;
				border_left = 0;
		} else {
				// black borders left/right
				height = game2->grid.height + 1.0f;
				width = height * aspect;
				border_left = (width - game2->grid.width) / 2;
				border_bottom = 0;
		}

		/*
		{
				static int foo = 0;
				if(!foo)
						printf("w,h: (%f,%f), border l/b: (%f,%f)\n",
									 width, height, border_left, border_bottom);
				foo = 1;
		}
		*/

		glViewport(d->vp_x, d->vp_y, d->vp_w, d->vp_h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, 0, height, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(border_left, border_bottom, 0);

		{
			float w = game2->grid.width;
			float h = game2->grid.height;

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glBegin(GL_QUADS);
			glColor4f(0.7f, 0.7f, 0.7f, 0.3f);
			glVertex2f(0, 0);
			glVertex2f(w, 0);
			glVertex2f(w, h);
			glVertex2f(0, h);
			glEnd();
		}
		
		glDisable(GL_LIGHTING);
		// glDisable(GL_BLEND);
		glEnable(GL_BLEND);
		{
				float w = game2->grid.width;
				float h = game2->grid.height;
				glColor3f(1, 1, 1);
				glBegin(GL_LINE_LOOP);
				glVertex3f( 0, 0, 0 );
				glVertex3f( w, 0, 0 );
				glVertex3f( w, h, 0 );
				glVertex3f( 0, h, 0 );
				glEnd();
		}
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
}
	
