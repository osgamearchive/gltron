#include "gltron.h"

/* draw a 2d map */

void draw2D( gDisplay *d ) {
		float width, height;
		float border_left, border_bottom;
		int i;

		float aspect = (float)d->vp_w / (float)d->vp_h;
		if(d->vp_w / game2->grid.width < d->vp_h / game2->grid.height) {
				// black borders top/bottom
				width = game2->grid.width + 1;
				height = width / aspect;
				border_bottom = (height - game2->grid.height) / 2;
				border_left = 0;
		} else {
				// black borders left/right
				height = game2->grid.height + 1;
				width = height * aspect;
				border_left = (width - game2->grid.width) / 2;
				border_bottom = 0;
		}

		{
				static int foo = 0;
				if(!foo)
						printf("w,h: (%f,%f), border l/b: (%f,%f)\n",
									 width, height, border_left, border_bottom);
				foo = 1;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, 0, height, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(border_left, border_bottom, 0);
		
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
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
				Line* trail;
				
				// fixme: check if trails vanish
				if(p->data->speed < 0)
						continue;
				
				glColor3fv( p->pColorDiffuse );
				glPointSize(2);
				glBegin(GL_POINTS);
				glVertex2f(p->data->iposx, p->data->iposy);
				glEnd();

				glBegin(GL_LINES);
				for(trail = p->data->trails; trail != p->data->trail; trail++)
				{
						glVertex2f(trail->sx, trail->sy);
						glVertex2f(trail->ex, trail->ey);
				}
				if(trail != p->data->trails)
				{
						trail--;
						glVertex2f(trail->ex, trail->ey);
						glVertex2f(p->data->iposx, p->data->iposy);
				}
				else
				{
						glVertex2f(trail->sx, trail->sy);
						glVertex2f(p->data->iposx, p->data->iposy);
				}
				glEnd();
		}
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
		
