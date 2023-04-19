//! @file alpha_blending.h

#ifndef ALPHA_BLENDING
#define ALPHA_BLENDING

//-----------------------------------------------------------------------------

#include "common.h"

//-----------------------------------------------------------------------------

const __m128i null_vector = _mm_set1_epi8 (0); 

const __m128i shuffle_mask_alpha = _mm_set_epi8 (0x80, 14, 0x80, 14, 0x80, 14, 0x80, 14, 
                                                 0x80,  6, 0x80,  6, 0x80,  6, 0x80,  6 );

const __m128i shuffle_mask_sum = _mm_set_epi8 (0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
                                               15,   13,   11,   9,    7,    5,    3,    1    );

//-----------------------------------------------------------------------------

void alpha_blending_slow (Image_info *res_image_info, 
                          Image_info  back_image_info, 
                          Image_info  front_image_info);

void alpha_blending_fast (Image_info *res_image_info, 
                          Image_info  back_image_info, 
                          Image_info  front_image_info);

//-----------------------------------------------------------------------------

#endif //ALPHA_BLENDING