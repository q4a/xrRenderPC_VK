#include "common.h"
#include "factory/ui/sequence_video_item.h"


/**
 *
 */
void
fUISequenceVideoItem::Copy
        ( IUISequenceVideoItem& obj
        )
{
    *this = *((fUISequenceVideoItem *)&obj);
}


/*
 *
 */
bool
fUISequenceVideoItem::HasTexture()
{
    // TBI
    return false;
}


/**
 *
 */
void
fUISequenceVideoItem::CaptureTexture()
{
    // TBI
}


/**
 *
 */
void
fUISequenceVideoItem::ResetTexture()
{
    // TBI
}


/**
 *
 */
BOOL
fUISequenceVideoItem::video_IsPlaying()
{
    // TBI
    return false;
}


/**
 *
 */
void
fUISequenceVideoItem::video_Sync
        ( u32 time
        )
{
    // TBI
}


/**
 *
 */
void
fUISequenceVideoItem::video_Play
    ( BOOL looped
    , u32 time /* = 0xFFFFFFFF */
    )
{
    // TBI
}

/**
 *
 */
void
fUISequenceVideoItem::video_Stop()
{
    // TBI
}
