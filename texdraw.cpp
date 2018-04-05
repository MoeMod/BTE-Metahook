#include "metahook.h"
#include "gl/gl.h"
#include "triangleapi.h"
#include "TriAPI.h"

void TEXD_BeginDraw(void)
{
	Tri_Enable(GL_TEXTURE_2D);
}

void TEXD_EndDraw(void)
{
	//Tri_Disable(GL_TEXTURE_2D);
}

void TEXD_SetColor(int r, int g, int b, int a)
{
	glColor4ub(r, g, b, a);
}

void TEXD_SetColor(colorVec color)
{
	glColor4ub(color.r, color.g, color.b, color.a);
}

void TEXD_SetRenderMode(int mode)
{
	switch (mode)
	{
	case kRenderNormal:
	default:
	{
		       glDisable(GL_BLEND);
		       glDisable(GL_ALPHA_TEST);
		       glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		       break;
	}

	case kRenderTransColor:
	{
				      glEnable(GL_BLEND);
				      glDisable(GL_ALPHA_TEST);
				      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				      break;
	}

	case kRenderTransAlpha:
	{
				      glDisable(GL_BLEND);
				      glEnable(GL_ALPHA_TEST);
				      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				      break;
	}

	case kRenderTransTexture:
	{
					glEnable(GL_BLEND);
					glDisable(GL_ALPHA_TEST);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

					break;
	}

	case kRenderGlow:
	{
				glEnable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				break;
	}

	case kRenderTransAdd:
	{
				    glEnable(GL_BLEND);
				    glDisable(GL_ALPHA_TEST);
				    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				    break;
	}
	}
}

void TEXD_BindTexture(int texid)
{
	Tri_BindTexture(GL_TEXTURE_2D, texid);
	Tri_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x8370);
	Tri_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x8370);
}

void TEXD_DrawTexture(int x, int y, float w, float h, wrect_t *prc, float size, int mode)
{
	float s1, s2, t1, t2;

	if (prc)
	{
		switch (mode)
		{
		case 0:
		{
			      s1 = (float)prc->left / w;
			      t1 = (float)prc->top / h;
			      s2 = (float)prc->right / w;
			      t2 = (float)prc->bottom / h;

			      break;
		}
		case 1:
		{
			      s2 = (float)prc->left / w;
			      t1 = (float)prc->top / h;
			      s1 = (float)prc->right / w;
			      t2 = (float)prc->bottom / h;

			      break;
		}
		case 2:
		{
			      s1 = (float)prc->left / w;
			      t2 = (float)prc->top / h;
			      s2 = (float)prc->right / w;
			      t1 = (float)prc->bottom / h;

			      break;
		}
		case 3:
		{
			      s2 = (float)prc->left / w;
			      t2 = (float)prc->top / h;
			      s1 = (float)prc->right / w;
			      t1 = (float)prc->bottom / h;

			      break;
		}
		}
		w = prc->right - prc->left;
		h = prc->bottom - prc->top;
	}
	else
	{
		s1 = t1 = 0;
		s2 = t2 = 1;
	}

	w *= size;
	h *= size;

	glBegin(GL_QUADS);
	glTexCoord2f(s1, t1);
	glVertex2f(x, y);

	glTexCoord2f(s2, t1);
	glVertex2f(x + w, y);

	glTexCoord2f(s2, t2);
	glVertex2f(x + w, y + h);

	glTexCoord2f(s1, t2);
	glVertex2f(x, y + h);
	glEnd();
}

void TEXD_DrawTexture2(int x, int y, float w, float h, float size, int mode)
{
	float s1, s2, s3, s4, t1, t2, t3, t4;

	switch (mode)
	{
	case 0:
	{
		      s1 = 0;	t1 = 0;
		      s2 = 1;	t2 = 0;
		      s3 = 1;	t3 = 1;
		      s4 = 0;	t4 = 1;

		      break;
	}
	case 1:
	{
		      int a = w;
		      w = h;
		      h = a;

		      s1 = 0;	t1 = 1;
		      s2 = 0;	t2 = 0;
		      s3 = 1;	t3 = 0;
		      s4 = 1;	t4 = 1;

		      break;
	}
	case 2:
	{
		      s1 = 1;	t1 = 1;
		      s2 = 0;	t2 = 1;
		      s3 = 0;	t3 = 0;
		      s4 = 1;	t4 = 0;

		      break;
	}
	case 3:
	{
		      int a = w;
		      w = h;
		      h = a;

		      s1 = 1;	t1 = 0;
		      s2 = 1;	t2 = 1;
		      s3 = 0;	t3 = 1;
		      s4 = 0;	t4 = 0;

		      break;
	}
	}

	w *= size;
	h *= size;

	glBegin(GL_QUADS);
	glTexCoord2f(s1, t1);
	glVertex2f(x, y);

	glTexCoord2f(s2, t2);
	glVertex2f(x + w, y);

	glTexCoord2f(s3, t3);
	glVertex2f(x + w, y + h);

	glTexCoord2f(s4, t4);
	glVertex2f(x, y + h);
	glEnd();
}