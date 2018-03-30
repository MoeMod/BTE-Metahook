/*
*	model.cpp
*	2017/9/26 Xiaobaibai: Moved external texture -> TextureManager.cpp
*/

#include <metahook.h>
#include <plugins.h>

#include <studio.h>
#include <imagelib.h>
#include <exportfuncs.h>
#include <util.h>

#include "model.h"
#include "Client/TextureManager.h"

void(*g_pfnMod_LoadStudioModel)(model_t *mod, byte *buffer);
hook_t *g_phMod_LoadStudioModel;

typedef struct
{
	int numseq;
	int seqindex;
}
studioseqpack_t;

#define MAX_SEQUENCEPACKS 256

FileHandle_t g_hSequencePack[MAX_SEQUENCEPACKS];
unsigned int g_nSequenceSize[MAX_SEQUENCEPACKS];
int g_iSequenceNums = 0;

void Model_InstallHook()
{
	g_pfnMod_LoadStudioModel = (void(*)(model_t *, byte *))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, MOD_LOADSTUDIOMODEL_SIG, sizeof(MOD_LOADSTUDIOMODEL_SIG) - 1);

	if (!g_pfnMod_LoadStudioModel)
		g_pfnMod_LoadStudioModel = (void(*)(model_t *, byte *))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, MOD_LOADSTUDIOMODEL_SIG_NEW, sizeof(MOD_LOADSTUDIOMODEL_SIG_NEW) - 1);

	g_pfnMod_LoadStudioModel = (void(*)(model_t *, byte *))0x1D619E0;

	g_phMod_LoadStudioModel = g_pMetaHookAPI->InlineHook(g_pfnMod_LoadStudioModel, Mod_LoadStudioModel, (void *&)g_pfnMod_LoadStudioModel);
}


void Mod_LoadStudioModel(model_t *mod, byte *buffer)
{
	studiohdr_t *studiohdr = (studiohdr_t *)buffer;
	if (((studiohdr_t*)buffer)->version == 'l' + 't' + 'n' + 'd' + 'k' + 'l')	// ..
	{
		((studiohdr_t*)buffer)->version = 10;
		((studiohdr_t*)buffer)->boneindex &= ~0x400;
	}

	/*char newname[MAX_QPATH];
	strcpy(newname, mod->name);
	newname[strlen(newname) - 4] = 0;

	for (int i = 0; i < 256; i++)
	{
		char filename[MAX_QPATH];
		sprintf(filename, "%s%d.seq", newname, i + 1);

		g_hSequencePack[i] = g_pFileSystem->Open(filename, "rb");

		if (g_hSequencePack[i] == FILESYSTEM_INVALID_HANDLE)
			break;

		g_nSequenceSize[i] = g_pFileSystem->Size(g_hSequencePack[i]);
		g_iSequenceNums++;
	}

	if (g_iSequenceNums > 0)
	{
		int totalsize = 0;

		for (int i = 0; i < g_iSequenceNums; i++)
			totalsize += g_nSequenceSize[i];

		if (totalsize)
		{
			static byte newbuf[0x500000];
			memset(newbuf, 0, sizeof(newbuf));
			memcpy(newbuf, buffer, studiohdr->length);
			buffer = newbuf;
			studiohdr = (studiohdr_t *)newbuf;
		}

		mstudiotexture_t *ptexture = (mstudiotexture_t *)((byte *)studiohdr + studiohdr->textureindex);
		int texdatasize = 0;

		for (int i = 0; i < studiohdr->numtextures; i++)
			texdatasize += ptexture[i].width * ptexture[i].height + 256 * 3;

		mstudiotexture_t *texbuf = (mstudiotexture_t *)malloc(studiohdr->numtextures * sizeof(mstudiotexture_t));
		memcpy(texbuf, ptexture, studiohdr->numtextures * sizeof(mstudiotexture_t));

		byte *texdatabuf = (byte *)malloc(texdatasize);
		memcpy(texdatabuf, (byte *)studiohdr + studiohdr->texturedataindex, texdatasize);

		int offset = 0;
		int endpos = studiohdr->texturedataindex;
		mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)studiohdr + studiohdr->seqindex);

		static byte seqdescbuf[0x2C000];
		memcpy(seqdescbuf, pseqdesc, sizeof(mstudioseqdesc_t) * studiohdr->numseq);

		int numseq = studiohdr->numseq;
		byte *endbuf = (byte *)studiohdr + endpos;
		byte *newbuf = seqdescbuf + (sizeof(mstudioseqdesc_t) * studiohdr->numseq);

		for (int i = 0; i < g_iSequenceNums; i++)
		{
			offset += sizeof(studioseqpack_t);

			if (g_pFileSystem->Read(endbuf, g_nSequenceSize[i], g_hSequencePack[i]) == -1)
			{
				g_pFileSystem->Close(g_hSequencePack[i]);
				continue;
			}

			studioseqpack_t *pack = (studioseqpack_t *)endbuf;
			mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)(endbuf + pack->seqindex);

			for (int j = 0; j < pack->numseq; j++)
			{
				pseqdesc[j].eventindex = endpos + offset;
				offset += pseqdesc[j].numevents * sizeof(mstudioevent_t);
				pseqdesc[j].pivotindex = endpos + offset;
				offset += pseqdesc[j].numpivots * sizeof(mstudiopivot_t);
			}

			for (int j = 0; j < pack->numseq; j++)
			{
				int animindex = endpos + offset;

				if (j == pack->numseq - 1)
					offset = (byte *)pseqdesc - endpos - (byte *)studiohdr;
				else
					offset += pseqdesc[j + 1].animindex - pseqdesc[j].animindex;

				pseqdesc[j].animindex = animindex;
			}

			memcpy(newbuf, pseqdesc, sizeof(mstudioseqdesc_t) * pack->numseq);
			newbuf += sizeof(mstudioseqdesc_t) * pack->numseq;
			numseq += pack->numseq;
			endbuf += pack->seqindex;

			g_pFileSystem->Close(g_hSequencePack[i]);
		}

		memcpy(endbuf, seqdescbuf, sizeof(mstudioseqdesc_t) * numseq);

		studiohdr->numseq = numseq;
		studiohdr->seqindex = endbuf - (byte *)studiohdr;

		endbuf += sizeof(mstudioseqdesc_t) * numseq;
		memcpy(endbuf, texbuf, sizeof(mstudiotexture_t) * studiohdr->numtextures);

		ptexture = (mstudiotexture_t *)endbuf;
		studiohdr->textureindex = endbuf - (byte *)studiohdr;
		endbuf += sizeof(mstudiotexture_t) * studiohdr->numtextures;
		texdatasize = 0;

		for (int i = 0; i < studiohdr->numtextures; i++)
		{
			ptexture[i].index = (endbuf + texdatasize) - (byte *)studiohdr;
			texdatasize += ptexture[i].width * ptexture[i].height + 256 * 3;
		}

		memcpy(endbuf, texdatabuf, texdatasize);

		studiohdr->texturedataindex = endbuf - (byte *)studiohdr;
		endbuf += texdatasize;
		studiohdr->length = endbuf - (byte *)studiohdr;

		free(texbuf);
		free(texdatabuf);

		g_iSequenceNums = 0;
	}
	*/
	g_pfnMod_LoadStudioModel(mod, buffer);

	studiohdr_t	*pstudio;
	mstudiotexture_t	*ptexture;
	pstudio = (studiohdr_t*)IEngineStudio.Mod_Extradata(mod);
	ptexture = (mstudiotexture_t *)(((byte *)pstudio) + pstudio->textureindex);

	for (int i = 0; i < pstudio->numtextures; i++)
	{
		if (!strnicmp(ptexture[i].name, "$0b", 3))
		{
			ptexture[i].flags |= STUDIO_NF_ADDITIVE;
		}
		if (!strnicmp(ptexture[i].name, "$20", 3))
		{
			ptexture[i].flags |= STUDIO_NF_FULLBRIGHT;
		}
		if (TextureManager().ReplaceTexture(mod->name, &ptexture[i]))
			continue;
	}
}