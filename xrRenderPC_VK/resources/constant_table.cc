#include "resources/constant_table.h"

void
ConstantTable::Merge
        ( const ConstantTable &source
        )
{
    // TODO: this can be a bit complicated. Check it later
    constants.insert( source.constants.cbegin()
                    , source.constants.cend()
    );
}
