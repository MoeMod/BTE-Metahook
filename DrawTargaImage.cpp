#include "base.h"
#include "configs.h"
#include "BaseUI.h"
#include "TriAPI.h"
#include "DrawTargaImage.h"

int GL_DrawTGA(int index,int r,int g,int b,int a,int iX,int iY,float scale)
{
	if (!index)
		return 0;
	vgui::surface()->DrawSetTexture(index);
	vgui::surface()->DrawSetColor(r,g,b,a);
	int w,h;
	vgui::surface()->DrawGetTextureSize(index,w,h);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vgui::surface()->DrawTexturedRect(iX,iY,iX+w * scale,iY+h * scale);	

	return 1;
}
int GL_DrawTGA2(int index, int iX, int iY, int iW, int iH, int alpha)
{
	if (!index) return 0;
	vgui::surface()->DrawSetTexture(index);
	vgui::surface()->DrawSetColor(255,255,255,alpha);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vgui::surface()->DrawTexturedRect(iX,iY,iX+iW,iY+iH);

	return 1;
}

int GL_DrawFullPanel(int tid[3][3], int iX, int iY, int iW, int iH, int alpha)
{
	if (!tid[0][0]) return 0;
	
	int length, width, width2, height, height2;
	vgui::surface()->DrawGetTextureSize(tid[0][0],width,height);
	vgui::surface()->DrawGetTextureSize(tid[0][2],width2,height2);
	length = iW - width - width2;

	GL_DrawTGA2(tid[0][0], iX, iY, width, height, alpha);
	GL_DrawTGA2(tid[0][1], iX + width, iY, length, height, alpha);
	GL_DrawTGA2(tid[0][2], iX + length + width, iY, width, height, alpha);

	iY += height;
	vgui::surface()->DrawGetTextureSize(tid[0][0],width2,height2);
	vgui::surface()->DrawGetTextureSize(tid[2][0],width,height);
	height = iH - height - height2;
	//width = g_MHTga[tid[1][0]].width;	
	vgui::surface()->DrawGetTextureSize(tid[1][0],width2,height2);
	vgui::surface()->DrawGetTextureSize(tid[1][2],width,height2);
	length = iW - width2 - width;

	GL_DrawTGA2(tid[1][0], iX, iY, width, height, alpha);
	GL_DrawTGA2(tid[1][1], iX + width, iY, length, height, alpha);
	GL_DrawTGA2(tid[1][2], iX + length + width, iY, width, height, alpha);

	iY += height;
	vgui::surface()->DrawGetTextureSize(tid[2][0],width,height);
	vgui::surface()->DrawGetTextureSize(tid[2][2],width2,height2);
	length = iW - width - width2;

	GL_DrawTGA2(tid[2][0], iX, iY, width, height, alpha);
	GL_DrawTGA2(tid[2][1], iX + width, iY, length, height, alpha);
	GL_DrawTGA2(tid[2][2], iX + length + width, iY, width, height, alpha);

	return 1;
}

int GL_DrawPanel(int tid[3], int iX, int iY, int iW, int iH, int alpha)
{
	if (!tid[0]) return 0;
	
	int length, height, width[2];
	vgui::surface()->DrawGetTextureSize(tid[0],width[0],height);
	vgui::surface()->DrawGetTextureSize(tid[2],width[1],height);
	height = iH;
	length = iW - width[0] - width[1];

	GL_DrawTGA2(tid[0], iX, iY, width[0], height, alpha);	
	GL_DrawTGA2(tid[1], iX + width[0], iY, length, height, alpha);
	GL_DrawTGA2(tid[2], iX + length + width[0], iY, width[1], height, alpha);


	return 1;
}
void GL_LoadFullPanel(int tid[3][3], char *szName)
{
	char str[3][10]={"top","center","bottom"};
	char str2[3][10]={"left","center","right"};

	for (int i = 0;i <= 2; i++)
	{
		for (int j = 0;j <= 2; j++)
		{
			char name[64];
			sprintf(name,"gfx\\ui\\%s\\%s_%s",szName,str[i],str2[j]);
			tid[i][j] = vgui::surface()->CreateNewTextureID();
			vgui::surface()->DrawSetTextureFile(tid[i][j],name,true,true);
		}
	}

}
void GL_LoadPanel(int tid[3], char *szName)
{
	char str[3][10]={"left","center","right"};

	for (int i = 0;i <= 2; i++)
	{
		char name[64];
		sprintf(name,"gfx\\ui\\%s\\%s",szName,str[i]);
		tid[i] = vgui::surface()->CreateNewTextureID();
		vgui::surface()->DrawSetTextureFile(tid[i],name,true,true);
	}
}