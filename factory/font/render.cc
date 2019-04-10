/*!
 * \file    render.cc
 * \brief   Font render
 *
 * NOTE: Render routine code is 1-1 copied from the original source.
 *       Need to be reviewed.
 */

#include "xrCore/Text/StringConversion.hpp"

#include "device/device.h"
#include "backend/backend.h"
#include "backend/streams.h"
#include "backend/vertex_formats.h"
#include "resources/manager.h"

#include "factory/font/render.h"

extern ENGINE_API Fvector2 g_current_font_scale; // TODO: Wow! Definitely should be reworked

/**
 *
 */
void
fFontRender::Initialize
        ( LPCSTR shader_name
        , LPCSTR texture_name
        )
{
    shader_ = resources.CreateShader(shader_name, texture_name);
}

/**
 *
 */
void
fFontRender::OnRender
        ( CGameFont &owner
        )
{
    if (!shader_)
    {
        // TODO: possibly the FontRender will use nullptr to draw
        //       new strings with old pipeline binding. If so we can
        //       loose some text output.
        return;
    }
    
    backend.SetShader(shader_);

    // Check if owner object need to be initialized
    if (!(owner.uFlags & CGameFont::fsValid))
    {
        const auto &texture = backend.GetActiveTexture(0);

        const int width    = texture->image->extent.width;
        const int height   = texture->image->extent.height;
        
        owner.vTS.set(width, height);
        owner.fTCHeight = owner.fHeight / float(owner.vTS.y);
        owner.uFlags |= CGameFont::fsValid;
    }

    auto &vertex_buffer = backend.vertex_stream;

    for (u32 i = 0; i < owner.strings.size();)
    {
        // calculate first-fit
        int count = 1;

        int length = owner.smart_strlen(owner.strings[i].string);

        while ((i + count) < owner.strings.size())
        {
            int L = owner.smart_strlen(owner.strings[i + count].string);

            if ((L + length) < MAX_MB_CHARS)
            {
                count++;
                length += L;
            }
            else
                break;
        }

        std::size_t offset_start = 0;
        vertex_buffer >> offset_start;

        // fill vertices
        u32 last = i + count;
        for (; i < last; i++)
        {
            CGameFont::String& PS = owner.strings[i];
            wchar_t wsStr[MAX_MB_CHARS];

            int len = owner.IsMultibyte() ? mbhMulti2Wide(wsStr, nullptr, MAX_MB_CHARS, PS.string) : xr_strlen(PS.string);

            if (len)
            {
                float X = float(iFloor(PS.x));
                float Y = float(iFloor(PS.y));
                float S = PS.height * g_current_font_scale.y;
                float Y2 = Y + S;
                float fSize = 0;

                if (PS.align)
                    fSize = owner.IsMultibyte() ? owner.SizeOf_(wsStr) : owner.SizeOf_(PS.string);

                switch (PS.align)
                {
                case CGameFont::alCenter: X -= (iFloor(fSize * 0.5f)) * g_current_font_scale.x; break;
                case CGameFont::alRight: X -= iFloor(fSize); break;
                }

                u32 clr, clr2;
                clr2 = clr = PS.c;
                if (owner.uFlags & CGameFont::fsGradient)
                {
                    u32 _R = color_get_R(clr) / 2;
                    u32 _G = color_get_G(clr) / 2;
                    u32 _B = color_get_B(clr) / 2;
                    u32 _A = color_get_A(clr);
                    clr2 = color_rgba(_R, _G, _B, _A);
                }

                // Vertex shader will cancel a DX9 correction, so make fake offset
                X -= 0.5f;
                Y -= 0.5f;
                Y2 -= 0.5f;

                vertex_format::TriangleList vertex;

                float tu, tv;
                for (int j = 0; j < len; j++)
                {
                    Fvector l;

                    l = owner.IsMultibyte() ? owner.GetCharTC(wsStr[1 + j]) : owner.GetCharTC((u16)(u8)PS.string[j]);

                    float scw = l.z * g_current_font_scale.x;

                    float fTCWidth = l.z / owner.vTS.x;

                    if (!fis_zero(l.z))
                    {
                        tu = (l.x / owner.vTS.x);
                        tv = (l.y / owner.vTS.y);

                        // NOTE: DX and VK have different coordinate systems
                        //       in terms of Y axis direction. Probably, it's
                        //       possible to correct this with UBO constants
                        //       only.
                        const auto y_corrected  = float(hw.draw_rect.height) -  Y;
                        const auto y2_corrected = float(hw.draw_rect.height) - Y2;

                        vertex.Set(X, y2_corrected, clr2, tu, tv + owner.fTCHeight);
                        vertex_buffer << vertex;

                        vertex.Set(X, y_corrected, clr, tu, tv);
                        vertex_buffer << vertex;

                        vertex.Set(X + scw, y2_corrected, clr2, tu + fTCWidth, tv + owner.fTCHeight);
                        vertex_buffer << vertex;

                        vertex.Set(X + scw, y_corrected, clr, tu + fTCWidth, tv);
                        vertex_buffer << vertex;
                    }
                    X += scw * owner.vInterval.x;
                    if (owner.IsMultibyte())
                    {
                        X -= 2;
                        if (IsNeedSpaceCharacter(wsStr[1 + j]))
                            X += owner.fXStep;
                    }
                }
            }
        }

        std::size_t offset_end = 0;
        vertex_buffer >> offset_end;

        const auto num_vertices =
            (offset_end - offset_start) / vertex_format::TriangleList::size;
        const auto num_primitives = num_vertices / triangles_per_quad;

        if (num_vertices)
        {
            backend.SetGeometry(vertex_buffer, backend.index_cache);
            backend.DrawIndexed(num_primitives);
        }
    }
}
