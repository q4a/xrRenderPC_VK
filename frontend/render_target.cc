/*!
 * \file    render_target.cc
 * \brief   Render targets manager implementation
 */

#include "device/device.h"
#include "resources/manager.h"

#include "frontend/render_target.h"

const char *attachment__generic0 = { "$user$generic0" }; // TODO: move into header. BC also needs it


/*!
 * \brief   Default class constructor
 */
RenderTarget::RenderTarget()
{
    const auto width  = hw.draw_rect.width;
    const auto height = hw.draw_rect.height;
}


/*!
 * \brief   Returns width of render target surface
 */
u32
RenderTarget::get_width()
{
    return hw.draw_rect.width;
}


/*!
 * \brief   Returns height of render target surface
 */
u32
RenderTarget::get_height()
{
    return hw.draw_rect.height;
}
