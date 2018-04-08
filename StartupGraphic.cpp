/****
 * 
 * Copyright (c) 2017, Crsky
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
****/

#include <metahook.h>
#include <GdiPlus.h>
#include "plugins.h"
#include "LoadTGA.h"

#pragma comment(lib, "gdiplus.lib")

#define BUFFER_SIZE ( 1024 * 1024 * 4 )

bool DrawLoadingImage( HWND window, RECT *rect )
{
	byte	*bits0, *bits1;
	int		wide, tall;
	int		x, y;

	bits0 = (byte *)malloc( BUFFER_SIZE );
	bits1 = (byte *)malloc( BUFFER_SIZE );

	if ( !bits0 || !bits1 )
	{
		if ( bits0 )
			free( bits0 );

		if ( bits1 )
			free( bits1 );

		return false;
	}

	if ( !LoadTGA( "resource/background/background_loading.tga", bits0, BUFFER_SIZE, &wide, &tall ) )
	{
		if ( bits0 )
			free( bits0 );

		if ( bits1 )
			free( bits1 );

		return false;
	}

	for ( y = 0; y < tall; y++ )
	{
		for ( x = 0; x < wide; x++ )
		{
			byte *src = &bits0[ ( ( ( tall - y - 1 ) * wide ) + x ) * 4 ];
			byte *dst = &bits1[ ( ( y * wide ) + x ) * 4 ];

			dst[0] = src[2];	// B
			dst[1] = src[1];	// G
			dst[2] = src[0];	// R
			dst[3] = src[3];	// A
		}
	}

	BITMAPINFOHEADER hdr;
	hdr.biSize = sizeof( BITMAPINFOHEADER );
	hdr.biWidth = wide;
	hdr.biHeight = tall;
	hdr.biPlanes = 1;
	hdr.biBitCount = 32;
	hdr.biCompression = BI_RGB;
	hdr.biSizeImage = 0;
	hdr.biXPelsPerMeter = 0;
	hdr.biYPelsPerMeter = 0;
	hdr.biClrUsed = 0;
	hdr.biClrImportant = 0;

	Gdiplus::Bitmap image( (BITMAPINFO *)&hdr, bits1 );

	Gdiplus::Graphics graphics( window );

	graphics.DrawImage( &image, Gdiplus::RectF( 0, 0, rect->right, rect->bottom ), 0, 0, 1024, 768, Gdiplus::UnitPixel );

	Gdiplus::Font font( L"SimSun", 9, Gdiplus::FontStyleBold, Gdiplus::UnitPoint );

	// Outline color
	Gdiplus::SolidBrush brush0( Gdiplus::Color( 0x00, 0x00, 0x00 ) );
	// String color
	Gdiplus::SolidBrush brush1( Gdiplus::Color( 0xFF, 0xFF, 0xFF ) );

	PWCHAR str = L"Loading ...";

	x = 580.0 * ( rect->right / 800.0 );
	y = 540.0 * ( rect->bottom / 600.0 );

	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x - 1, y ), NULL, &brush0 );
	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x + 1, y ), NULL, &brush0 );
	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x, y + 1 ), NULL, &brush0 );
	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x, y - 1 ), NULL, &brush0 );

	graphics.DrawString( str, -1, &font, Gdiplus::PointF( x + 0, y ), NULL, &brush1 );

	// »­Ô²½Ç¾ØÐÎ
	y -= 5.0;
	Gdiplus::Color lineColor(150, 75, 75, 75);
	int lineWidth = 2;
	
	int arcSize = 4;
	int Width = 180;
	int Height = 36;
	int hew = Width / arcSize / 2;
	int heh = Height / arcSize / 2;
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

	Gdiplus::GraphicsPath roundRectPath;
	roundRectPath.AddLine(x + hew, y, x + Width - hew, y);  // ¶¥²¿ºáÏß  
	roundRectPath.AddArc(x + Width - 2 * hew, y, 2 * hew, 2 * heh, 270, 90); // ÓÒÉÏÔ²½Ç  
	roundRectPath.AddLine(x + Width, y + heh, x + Width, y + Height - heh);  // ÓÒ²àÊúÏß  
	roundRectPath.AddArc(x + Width - 2 * hew, y + Height - 2 * heh, 2 * hew, 2 * heh, 0, 90); // ÓÒÏÂÔ²½Ç  
	roundRectPath.AddLine(x + Width - hew, y + Height, x + hew, y + Height);  // µ×²¿ºáÏß  
	roundRectPath.AddArc(x, y + Height - 2 * heh, 2 * hew, 2 * heh, 90, 90); // ×óÏÂÔ²½Ç  
	roundRectPath.AddLine(x, y + Height - heh, x, y + heh);  // ×ó²àÊúÏß  
	roundRectPath.AddArc(x, y, 2 * hew, 2 * heh, 180, 90); // ×óÉÏÔ²½Ç  
	Gdiplus::Pen pen(lineColor, lineWidth);
	graphics.DrawPath(&pen, &roundRectPath);
	Gdiplus::SolidBrush brush(lineColor);
	graphics.FillPath(&brush, &roundRectPath);

	if ( bits0 )
		free( bits0 );

	if ( bits1 )
		free( bits1 );
	return true;
}

void(__fastcall *g_pfn_CVideoMode_Common__DrawStartupGraphic)(void *pthis, int edx, HWND hwnd) = (void(__fastcall *)(void *pthis, int edx, HWND window)) 0x01DBCA00;

// CVideoMode_Common::DrawStartupGraphic( HWND window )
void __fastcall CVideoMode_Common__DrawStartupGraphic( void *pthis, int edx, HWND window )
{
	RECT	rc;

	GetClientRect( window, &rc );

	Gdiplus::GdiplusStartupInput gdipStartupInput;
	ULONG gdipToken;

	bool bDrawn = false;

	if ( Gdiplus::GdiplusStartup( &gdipToken, &gdipStartupInput, NULL ) == Gdiplus::Status::Ok )
	{
		if (DrawLoadingImage(window, &rc))
			bDrawn = true;
		Gdiplus::GdiplusShutdown( gdipToken );
	}

	if (!bDrawn)
		return g_pfn_CVideoMode_Common__DrawStartupGraphic(pthis, edx, window);
}

void StartupGraphic_Init( void )
{
	g_pMetaHookAPI->InlineHook(g_pfn_CVideoMode_Common__DrawStartupGraphic, CVideoMode_Common__DrawStartupGraphic, (void *&)g_pfn_CVideoMode_Common__DrawStartupGraphic );
}
