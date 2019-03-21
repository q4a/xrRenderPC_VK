/*!
 * \file    default_bindings.cc
 * \brief   Default bindings for shader constants
 */

#include "device/device.h"
#include "resources/blender_compiler.h"
#include "resources/constant_table.h"


/*!
 * \brief   Updates `screen_res` vector constant
 * \param [in] table    UBO
 * \param [in] member   member parameters
 */
void
Update__screen_res
        ( const ConstantTable           &table
        , ConstantTable::ShaderConstant &member
        )
{
    const glm::vec4 screen_res{ hw.draw_rect.width
                              , hw.draw_rect.height
                              , 1.0f / hw.draw_rect.width
                              , 1.0f / hw.draw_rect.height
    };
    const auto crc = crc32(&screen_res, sizeof(screen_res));

    if (crc == member.crc)
    {
        // No data changed. Skip transfering.
        return;
    }

    for (auto index = 0; index < hw.baseRt.size(); index++)
    {
        const auto &buffer = table.buffers[index];
        std::uint8_t * const buffer_pointer =
            static_cast<std::uint8_t *>(buffer->GetHostPointer());


        CopyMemory( buffer_pointer + member.offset
                  , &screen_res
                  , member.size
        );

        buffer->position_ += sizeof(screen_res);
        buffer->Sync();

        // reset buffer offset
        buffer->offset_ = 0;
    }

    member.crc = crc;
}


/*!
 * \brief   Maps binders for default uniforms
 * \param [in] table buffer to udpate
 */
void
BlenderCompiler::SetDefaultBindings(ConstantTable &table) const
{
    const auto &screen_res = table.members.find("screen_res");
    if (screen_res != table.members.cend())
    {
        screen_res->second.Update =
            std::bind( Update__screen_res
                     , std::cref(table)
                     , std::ref(screen_res->second)
            );
    }
}
