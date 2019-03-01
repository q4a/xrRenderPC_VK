#include "backend/streams.h"

#include "backend/backend.h"

/**
 *
 */
void
BackEnd::OnFrameBegin()
{
    vertex_stream_ << StreamControl::Reset;
    index_stream_  << StreamControl::Reset;
}


/**
 *
 */
void
BackEnd::OnFrameEnd()
{
    vertex_stream_ << StreamControl::Flush;
    index_stream_  << StreamControl::Flush;
}
