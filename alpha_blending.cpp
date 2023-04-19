#include "alpha_blending.h"
#include "image_utils.h"

//-----------------------------------------------------------------------------

#define CURR_POS y * back_image_info.width + x
#define CUR_BACK back_image_info.raster[CURR_POS]
#define CUR_FRONT front_image_info.raster[CURR_POS]
#define CUR_RES res_image_info->raster[CURR_POS]

//-----------------------------------------------------------------------------

#define BLEND(i)                                                                      \
    CUR_RES.i = (CUR_FRONT.i * CUR_FRONT.A + CUR_BACK.i * (SOLID - CUR_FRONT.A)) >> 8

void alpha_blending_slow (Image_info *res_image_info, 
                          Image_info  back_image_info, 
                          Image_info  front_image_info)
{
    for(int y = 0; y < back_image_info.height; y++)
    {
        for(int x = 0; x < back_image_info.width; x++)
        {
            BLEND (R);              
            BLEND (G);
            BLEND (B);            
            CUR_RES.A = SOLID; 
        }
    }
}

//-----------------------------------------------------------------------------

void alpha_blending_fast (Image_info *res_image_info, 
                          Image_info  back_image_info, 
                          Image_info  front_image_info)
{
    for(int y = 0; y < back_image_info.height; y++)
    {
        for(int x = 0; x < back_image_info.width; x += 4)
        {
            // load 4 colors from raster to reg128
            __m128i front_vector_l = _mm_load_si128 ((__m128i*)((uint32_t*) (&front_image_info.raster[CURR_POS])));
            __m128i back_vector_l  = _mm_load_si128 ((__m128i*)((uint32_t*) (&back_image_info.raster[CURR_POS])));

            // divide into 2 parts
            __m128i front_vector_h = (__m128i) _mm_movehl_ps ((__m128) null_vector, (__m128) front_vector_l);
            __m128i back_vector_h  = (__m128i) _mm_movehl_ps ((__m128) null_vector, (__m128) back_vector_l);
        
            // split by nulls (for multiplication)
            front_vector_l = _mm_cvtepi8_epi16 (front_vector_l);
            front_vector_h = _mm_cvtepi8_epi16 (front_vector_h);
            back_vector_l  = _mm_cvtepi8_epi16 (back_vector_l);
            back_vector_h  = _mm_cvtepi8_epi16 (back_vector_h);
            
            // calculate new color
            __m128i alpha_mask_l = _mm_shuffle_epi8 (front_vector_l, shuffle_mask_alpha);
            __m128i alpha_mask_h = _mm_shuffle_epi8 (front_vector_h, shuffle_mask_alpha);
        
            front_vector_l = _mm_mullo_epi16 (front_vector_l, alpha_mask_l);    
            front_vector_h = _mm_mullo_epi16 (front_vector_h, alpha_mask_h);   
            back_vector_l = _mm_mullo_epi16 (back_vector_l, _mm_sub_epi16 (_mm_set1_epi16(255), alpha_mask_l));    
            back_vector_h = _mm_mullo_epi16 (back_vector_h, _mm_sub_epi16 (_mm_set1_epi16(255), alpha_mask_h));  

            __m128i new_vector_l = _mm_add_epi16 (front_vector_l, back_vector_l);     
            __m128i new_vector_h = _mm_add_epi16 (front_vector_h, back_vector_h);   

            new_vector_l = _mm_shuffle_epi8 (new_vector_l, shuffle_mask_sum);
            new_vector_h = _mm_shuffle_epi8 (new_vector_h, shuffle_mask_sum);

            __m128i res_vector = (__m128i) _mm_movelh_ps ((__m128) new_vector_l, (__m128) new_vector_h);

            // upload it now
            _mm_store_si128 ((__m128i*) (&res_image_info->raster[CURR_POS]), res_vector); 
        }
    } 
}

//-----------------------------------------------------------------------------

#undef CURR_POS
#undef CUR_BACK
#undef CUR_RES
#undef CUR_FRONT
#undef BLEND

//-----------------------------------------------------------------------------