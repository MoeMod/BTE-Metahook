#include <metahook.h>
#include "configs.h"
#include "LoadTGA.h"
#include "plugins.h"
#include "perf_counter.h"
#include "cvardef.h"
#include "util.h"

#pragma pack(1)

typedef struct _TargaHeader
{;
	unsigned char id_length, colormap_type, image_type;
	unsigned short colormap_index, colormap_length;
	unsigned char colormap_size;
	unsigned short x_origin, y_origin, width, height;
	unsigned char pixel_size, attributes;
}
TargaHeader;

#pragma pack()

int LoadTGA(const char *name, byte *targa_rgba, int bufferSize, int *width, int *height, int FlipTga)
{
	FileHandle_t fp = g_pFileSystem->Open(name, "rb");

	if (!fp)
	{
		LogToFile("LoadTGA() : Fail Load %s (File not found)", name);
		return FALSE;
	}

	bool debugTime = (developer && (int)developer->value > 2);
	float startTime;

	if (debugTime)
		startTime = gPerformanceCounter.GetCurTime();

	int columns, rows, numPixels;
	byte *pixbuf;
	int row, col;
	TargaHeader targa_header;

	if (!g_pFileSystem->Read(&targa_header, sizeof(targa_header), fp))
	{
		*width = 0;
		*height = 0;

		g_pFileSystem->Close(fp);
		LogToFile("LoadTGA() : Fail Load %s (Error header format)", name);
		return FALSE;
	}

	columns = targa_header.width;
	rows = targa_header.height;
	numPixels = columns * rows;

	*width = targa_header.width;
	*height = targa_header.height;

	if (targa_header.id_length != 0)
		g_pFileSystem->Seek(fp, targa_header.id_length, FILESYSTEM_SEEK_CURRENT);

	int savepos = g_pFileSystem->Tell(fp);
	int filesize = g_pFileSystem->Size(fp);

	int size = filesize - savepos;
	byte *freebuf = nullptr;
	try{
		freebuf = new byte[size];
	}
	catch (...)
	{
		LogToFile("LoadTGA() : Fail Load %s (operator new : Bad Alloc)", name);
		g_pFileSystem->Close(fp);
		return FALSE;
	}
	byte *buf_p = freebuf;
	int readcount = g_pFileSystem->Read(buf_p, size, fp);
	g_pFileSystem->Close(fp);

	if (!readcount)
	{
		delete[] freebuf;
		LogToFile("LoadTGA() : Fail Load %s (incomplete file)", name);
		return FALSE;
	}

	int	i, row_inc;
	byte	palette[256][4], red = 0, green = 0, blue = 0, alpha = 0;
	int	readpixelcount, pixelcount;
	qboolean	compressed;

	if (targa_header.image_type == 1 || targa_header.image_type == 9)
	{
		// uncompressed colormapped image
		if (targa_header.pixel_size != 8)
		{
			LogToFile("LoadTGA() : Fail Load %s (Only 8 bit images supported for type 1 and 9)", name);
			delete[] freebuf;
			return FALSE;
		}
		if (targa_header.colormap_length != 256)
		{
			LogToFile("LoadTGA() : Fail Load %s (Only 8 bit colormaps are supported for type 1 and 9)", name);
			delete[] freebuf;
			return FALSE;
		}
		if (targa_header.colormap_index)
		{
			LogToFile("LoadTGA() : Fail Load %s (colormap_index is not supported for type 1 and 9)", name);
			delete[] freebuf;
			return FALSE;
		}
		if (targa_header.colormap_size == 24)
		{
			for (i = 0; i < targa_header.colormap_length; i++)
			{
				palette[i][2] = *buf_p++;
				palette[i][1] = *buf_p++;
				palette[i][0] = *buf_p++;
				palette[i][3] = 255;
			}
		}
		else if (targa_header.colormap_size == 32)
		{
			for (i = 0; i < targa_header.colormap_length; i++)
			{
				palette[i][2] = *buf_p++;
				palette[i][1] = *buf_p++;
				palette[i][0] = *buf_p++;
				palette[i][3] = *buf_p++;
			}
		}
		else
		{
			LogToFile("LoadTGA() : Fail Load %s (only 24 and 32 bit colormaps are supported for type 1 and 9)", name);
			return FALSE;
		}
	}
	else if (targa_header.image_type == 2 || targa_header.image_type == 10)
	{
		// uncompressed or RLE compressed RGB
		if (targa_header.pixel_size != 32 && targa_header.pixel_size != 24)
		{
			LogToFile("LoadTGA() : Only 32 or 24 bit images supported for type 2 and 10", name);
			delete[] freebuf;
			return FALSE;
		}
	}
	else if (targa_header.image_type == 3 || targa_header.image_type == 11)
	{
		// uncompressed greyscale
		if (targa_header.pixel_size != 8)
		{
			LogToFile("LoadTGA() : Only 8 bit images supported for type 3 and 11", name);
			delete[] freebuf;
			return FALSE;
		}
	}

	columns = targa_header.width;
	rows = targa_header.height;

	// if bit 5 of attributes isn't set, the image has been stored from bottom to top
	if (FlipTga && targa_header.attributes & 0x20)
	{
		pixbuf = targa_rgba;
		row_inc = 0;
	}
	else
	{
		pixbuf = targa_rgba + (rows - 1) * columns * 4;
		row_inc = -columns * 4 * 2;
	}

	compressed = (targa_header.image_type == 9 || targa_header.image_type == 10 || targa_header.image_type == 11);
	for (row = col = 0; row < rows;)
	{
		pixelcount = 0x10000;
		readpixelcount = 0x10000;

		if (compressed)
		{
			pixelcount = *buf_p++;
			if (pixelcount & 0x80)  // run-length packet
				readpixelcount = 1;
			pixelcount = 1 + (pixelcount & 0x7f);
		}

		while (pixelcount-- && (row < rows))
		{
			if (readpixelcount-- > 0)
			{
				switch (targa_header.image_type)
				{
				case 1:
				case 9:
					// colormapped image
					blue = *buf_p++;
					red = palette[blue][0];
					green = palette[blue][1];
					alpha = palette[blue][3];
					blue = palette[blue][2];
					//if( alpha != 255 ) image.flags |= IMAGE_HAS_ALPHA;
					break;
				case 2:
				case 10:
					// 24 or 32 bit image
					blue = *buf_p++;
					green = *buf_p++;
					red = *buf_p++;
					alpha = 255;
					//alpha = 255 * ((int)blue + (int)green + (int)red) / (255 * 3);

					if (targa_header.pixel_size == 32)
					{
						alpha = *buf_p++;
						//if( alpha != 255 )
						//image.flags |= IMAGE_HAS_ALPHA;
					}
					break;
				case 3:
				case 11:
					// greyscale image
					blue = green = red = *buf_p++;
					alpha = 255;
					break;
				}
			}


			*pixbuf++ = red;
			*pixbuf++ = green;
			*pixbuf++ = blue;
			*pixbuf++ = alpha;
			if (++col == columns)
			{
				row++;
				col = 0;
				pixbuf += row_inc;
			}
		}
	}

	
	*width = targa_header.width;
	*height = targa_header.height;

	if (debugTime)
		gEngfuncs.Con_Printf("LoadTGA: load %s take %.4f sec.\n", name, gPerformanceCounter.GetCurTime() - startTime);

	delete[] freebuf;
	return TRUE;
}

bool GetTGASize(const char *szFilename, int *width, int *height)
{
	FileHandle_t fp = g_pFileSystem->Open(szFilename, "rb");

	if (!fp)
		return FALSE;

	TargaHeader header;

	if (!g_pFileSystem->Read(&header, sizeof(header), fp))
	{
		*width = 0;
		*height = 0;
		return FALSE;
	}
	
	*width = header.width;
	*height = header.height;

	g_pFileSystem->Close(fp);
	return TRUE;
}

bool WriteTGA(byte *pixels, int width, int height, const char *szFilename)
{
	TargaHeader header;
	memset(&header, 0, sizeof(header));

	header.width = width;
	header.height = height;
	header.pixel_size = 24;
	header.attributes = 0x20;
	header.image_type = 2;

	char filename[MAX_PATH];
	sprintf(filename, "%s/%s", gConfigs.szGameLangDir, szFilename);

	FILE *fp = fopen(filename, "wb");

	if (!fp)
		return FALSE;

	if (!fwrite(&header, sizeof(header), 1, fp))
	{
		fclose(fp);
		return FALSE;
	}

	int numpixels = width * height;

	for (int i = 0; i < numpixels; i++)
	{
		register unsigned char color[3];

		color[0] = pixels[2];
		color[1] = pixels[1];
		color[2] = pixels[0];
		pixels += 3;

		if (!fwrite(color, 3, 1, fp))
		{
			fclose(fp);
			return FALSE;
		}
	}

	fclose(fp);
	return TRUE;
}