//! @file alpha_blending.h

#ifndef ALPHA_BLENDING
#define ALPHA_BLENDING

//-----------------------------------------------------------------------------

#include "common.h"

//-----------------------------------------------------------------------------

sf::Image alpha_blending_slow (Image_info image_dest_info, Image_info image_src_info);

sf::Image alpha_blending_fast (Image_info image_dest_info, Image_info image_src_info);

uint32_t find_min (uint32_t a1, uint32_t a2);

//-----------------------------------------------------------------------------

#endif //ALPHA_BLENDING