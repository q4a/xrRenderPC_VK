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
        ( const ConstantTable                 &table
        , const ConstantTable::ShaderConstant &member
        )
{
    for (auto index = 0; index < hw.baseRt.size(); index++)
    {
        const auto &buffer = table.buffers[index];
        void * const buffer_pointer =
            buffer->cpu_buffer_->allocation_info.pMappedData;

        glm::vec4 screen_res{ hw.draw_rect.width
                            , hw.draw_rect.height
                            , 1.0f / hw.draw_rect.width
                            , 1.0f / hw.draw_rect.height
        };

        auto * const pointer =
            static_cast<std::uint8_t *>(buffer_pointer) + member.offset;
        CopyMemory( pointer
                  , &screen_res
                  , member.size
        );

        buffer->position_ += sizeof(screen_res);
        buffer->Sync();

        // reset buffer offset
        buffer->offset_ = 0;
    }
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
        void Update__screen_res( const ConstantTable &
                               , const ConstantTable::ShaderConstant &
                               );
        screen_res->second.Update =
            std::bind( Update__screen_res
                     , std::cref(table)
                     , std::cref(screen_res->second)
            );
    }
}
