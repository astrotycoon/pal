/*
	Copyright (c) 2010 John McCutchan <john@johnmccutchan.com>

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#include "libpal/freetype/include/ft2build.h"
#include FT_FREETYPE_H

#include "libpal/libpal.h"
#include "libpal/pal_font_rasterizer_freetype.h"

#define FTL_CAST(x) (reinterpret_cast<FT_Library>(x))
#define FTF_CAST(x) (reinterpret_cast<FT_Face>(x))

palFontRasterizerFreeType::palFontRasterizerFreeType() {
  ft_library_ = NULL;
  ft_face_ = NULL;
}

palFontRasterizerFreeType::~palFontRasterizerFreeType() {
  if (ft_face_ != NULL) {
    FT_Done_Face(FTF_CAST(ft_face_));
  }

  if (ft_library_ != NULL) {
    FT_Done_FreeType(FTL_CAST(ft_library_));
  }
}

palFontRasterizerError palFontRasterizerFreeType::InitFont(const unsigned char* font_data,  int font_data_length, int face_index) {
  FT_Error e;
  
  FT_Library ftl = FTL_CAST(ft_library_);
  FT_Face ftf = FTF_CAST(ft_face_);

  e = FT_Init_FreeType(&ftl);
  if (e != 0) {
    return PAL_FONT_RASTERIZER_ERROR;
  }

  e = FT_New_Memory_Face(ftl, font_data, font_data_length, face_index, &ftf);
  if (e != 0) {
    FT_Done_FreeType(ftl);
    ft_library_ = NULL;
    return PAL_FONT_RASTERIZER_ERROR;
  }

  ft_library_ = ftl;
  ft_face_ = ftf;

  return PAL_FONT_RASTERIZER_NO_ERROR;
}

int palFontRasterizerFreeType::GetGlyphIndex(int ch) {
  if (!ft_face_)
    return 0;

  FT_Face ftf = FTF_CAST(ft_face_);

  int glyph = FT_Get_Char_Index(ftf, ch);
  return glyph;
}

void palFontRasterizerFreeType::GetFontExtents(palFontExtents* font_extents) {
  if (!ft_face_ || !font_extents)
    return;

  FT_Face ftf = FTF_CAST(ft_face_);

  font_extents->ascent = ftf->size->metrics.ascender >> 6;
  font_extents->descent = ftf->size->metrics.descender >> 6;
  font_extents->linegap = (ftf->size->metrics.height >> 6) - (ftf->size->metrics.ascender >> 6) + (ftf->size->metrics.descender >> 6);
  font_extents->next_line_delta = ftf->size->metrics.height >> 6;
}

int palFontRasterizerFreeType::GetKerningAdvance(int glyph1, int glyph2) {
  return 0;
}

void palFontRasterizerFreeType::SetHeightInPixels(int font_height_in_pixels) {
#if 1
  FT_Face ftf = FTF_CAST(ft_face_);
  FT_Error e;
  e = FT_Set_Pixel_Sizes(ftf, 0, font_height_in_pixels);
  palAssert(e == 0);
#else
  int pts = (int)((float)font_height_in_pixels/1.33333f);
  FT_Face ftf = FTF_CAST(ft_face_);
  FT_Error e;
  e = FT_Set_Char_Size(ftf, 0, pts*64, 96, 96);
  palAssert(e == 0);
#endif
}

void palFontRasterizerFreeType::LoadGlyph(int glyph) {
  if (glyph == 0)
    return;

  FT_Face ftf = FTF_CAST(ft_face_);
  FT_Error e;
  e = FT_Load_Glyph(ftf, glyph, FT_LOAD_DEFAULT);
  palAssert(e == 0);
}

void palFontRasterizerFreeType::GetGlyphExtents(palFontGlyphExtents* glyph_extents) {
  if (!glyph_extents)
    return;

  FT_Face ftf = FTF_CAST(ft_face_);
  glyph_extents->pre_x_offset = ftf->glyph->metrics.horiBearingX >> 6;
  glyph_extents->post_x_offset = ftf->glyph->metrics.horiAdvance >> 6;
}

void palFontRasterizerFreeType::RasterizeGlyph() {
  FT_Face ftf = FTF_CAST(ft_face_);
  FT_Error e;
  e = FT_Render_Glyph(ftf->glyph, FT_RENDER_MODE_NORMAL);
  palAssert(e == 0);
}

void palFontRasterizerFreeType::GetBitmap(unsigned char** pixels, int* bitmap_left, int* bitmap_top, int* width, int* height, int* pitch) {
  FT_Face ftf = FTF_CAST(ft_face_);
  *bitmap_left = ftf->glyph->bitmap_left;
  *bitmap_top = -ftf->glyph->bitmap_top;
  *pixels = ftf->glyph->bitmap.buffer;
  *width = ftf->glyph->bitmap.width;
  *height = ftf->glyph->bitmap.rows;
  *pitch = ftf->glyph->bitmap.pitch;
}

void palFontRasterizerFreeType::GetGlyphRasterBox(palFontGlyphRasterBox* raster_box) {
  if (!raster_box)
    return;

  FT_Face ftf = FTF_CAST(ft_face_);
  raster_box->min[0] = 0;
  raster_box->min[1] = 0;
  raster_box->max[0] = ftf->glyph->bitmap.width;
  raster_box->max[1] = ftf->glyph->bitmap.rows;
}