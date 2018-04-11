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
#include "IceKey.h"

#include "model.h"
#include "Client/TextureManager.h"

static CONST BYTE g_pDecryptorKey_20[32] =
{
	0x32, 0xA6, 0x21, 0xE0, 0xAB, 0x6B, 0xF4, 0x2C,
	0x93, 0xC6, 0xF1, 0x96, 0xFB, 0x38, 0x75, 0x68,
	0xBA, 0x70, 0x13, 0x86, 0xE0, 0xB3, 0x71, 0xF4,
	0xE3, 0x9B, 0x07, 0x22, 0x0C, 0xFE, 0x88, 0x3A
};

static CONST BYTE g_pDecryptorKey_21[32] =
{
	0x22, 0x7A, 0x19, 0x6F, 0x7B, 0x86, 0x7D, 0xE0,
	0x8C, 0xC6, 0xF1, 0x96, 0xFB, 0x38, 0x75, 0x68,
	0x88, 0x7A, 0x78, 0x86, 0x78, 0x86, 0x67, 0x70,
	0xD9, 0x91, 0x07, 0x3A, 0x14, 0x74, 0xFE, 0x22
};

CIceKey g_Decryptor(4);

VOID DecryptChunk(BYTE *pData, SIZE_T uDataSize)
{
	if (!uDataSize)
		return;

	SIZE_T uCount = (uDataSize + 7) >> 3;

	while (uCount)
	{
		g_Decryptor.Decrypt(pData, pData);
		pData += 8;
		uCount--;
	}
}

VOID DecryptData(BYTE *pData, SIZE_T uDataSize)
{
	if (!uDataSize)
		return;

	do
	{
		SIZE_T uTempSize = uDataSize;

		if (uTempSize > 1024)
			uTempSize = 1024;

		if (uTempSize & 7)
			return;

		DecryptChunk(pData, uTempSize);
		pData += uTempSize;
		uDataSize -= uTempSize;
	} while (uDataSize);
}

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

	if (!strncmp(mod->name, "models/player", 13))
	{
		if (studiohdr->numhitboxes == 21)
			studiohdr->numhitboxes = 20;
	}

	if (studiohdr->version == 'l' + 't' + 'n' + 'd' + 'k' + 'l')	// ..
	{
		studiohdr->version = 10;
		studiohdr->boneindex &= ~0x400;
	}

	if (studiohdr->version == 20 || studiohdr->version == 21)
	{
		if (studiohdr->version == 20)
			g_Decryptor.SetKey(g_pDecryptorKey_20);
		else if (studiohdr->version == 21)
			g_Decryptor.SetKey(g_pDecryptorKey_21);

		mstudiotexture_t *ptexture = (mstudiotexture_t *)(buffer + studiohdr->textureindex);

		for (int i = 0; i < studiohdr->numtextures; i++)
			DecryptData(buffer + ptexture[i].index, (ptexture[i].width * ptexture[i].height) + (256 * 3));

		mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)(buffer + studiohdr->bodypartindex);

		for (int i = 0; i < studiohdr->numbodyparts; i++)
		{
			mstudiomodel_t *pmodel = (mstudiomodel_t *)(buffer + pbodypart[i].modelindex);

			for (int j = 0; j < pbodypart[i].nummodels; j++)
			{
				if (pmodel[j].numverts > 0)
					DecryptData(buffer + pmodel[j].vertindex, pmodel[j].numverts * sizeof(vec3_t));
			}
		}

		studiohdr->version = 10;
	}

	char newname[MAX_QPATH];
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