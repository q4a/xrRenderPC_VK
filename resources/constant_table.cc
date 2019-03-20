/*!
 * \file    constant_table.cc
 * \brief   Constant table resource implementation
 */

#include "resources/manager.h"

#include "resources/constant_table.h"


/*!
 * \brief   Creates an unique constant table resource
 * \param [in] table reference table
 * \return pointer to fresh or previously created object
 */
std::shared_ptr<ConstantTable>
ResourceManager::CreateConstantTable
        ( const ConstantTable &table
        )
{
    /* Check if we already have the same table created */
    auto is_equal = [&](const auto &table_pointer) -> bool
    {
        return table.IsEqual(*table_pointer);
    };

    const auto &iterator =
        std::find_if( tables_.cbegin()
                    , tables_.cend()
                    , is_equal
        );

    if (iterator != tables_.cend())
    {
        return *iterator;
    }

    const auto pointer = std::make_shared<ConstantTable>(table);
    tables_.push_back(pointer);

    return pointer;
}


/*!
 * \brief   Checks whether constant tables are equal
 * \param [in] reference input constant table to compare with
 * \return operation result
 */
bool
ConstantTable::IsEqual
        (const ConstantTable &reference) const
{
    if (size != reference.size)
    {
        return false;
    }

    if (crc != reference.crc)
    {
        return false;
    }

    return true;
}
