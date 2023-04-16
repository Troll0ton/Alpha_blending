#include "alpha_blending.h"

//-----------------------------------------------------------------------------

#define CUR_DST image_dest_info.raster[CURR_POS]
#define CUR_SRC image_src_info.raster[CURR_POS]
#define CURR_POS y * image_dest_info.width + x

sf::Image alpha_blending_slow (Image_info image_dest_info, Image_info image_src_info)
{
    sf::Image image;

    image.create (image_dest_info.width, image_dest_info.height, sf::Color::Black);

    for(int y = 0; y < image_dest_info.height; y++)
    {
        for(int x = 0; x < image_dest_info.width; x++)
        {
            sf::Color color = sf::Color{(unsigned char) ((CUR_SRC.R * CUR_SRC.A + CUR_DST.R * (INC_ALPHA - CUR_DST.A)) >> 8),               
                                        (unsigned char) ((CUR_SRC.G * CUR_SRC.A + CUR_DST.G * (INC_ALPHA - CUR_DST.A)) >> 8),               
                                        (unsigned char) ((CUR_SRC.B * CUR_SRC.A + CUR_DST.B * (INC_ALPHA - CUR_DST.A)) >> 8),               
                                        SOLID}; 

            image.setPixel (x, y, color);   
        }
    }

    return image;
}

sf::Image alpha_blending_fast (Image_info image_dest_info, Image_info image_src_info)
{
    sf::Image image;

    image.create (image_dest_info.width, image_dest_info.height, sf::Color::Black);

    __m128i _m_zero = _mm_set1_epi8 (0); 

    uint32_t tmp_raster[4] = { 0 };
    sf::Color color = sf::Color { 0 };

    for(int y = 0; y < image_dest_info.height; y++)
    {
        for(int x = 0; x < image_dest_info.width; x += 4)
        {
        // Step 1 - Loading the values in vectors
            __m128i ForegrL = _mm_loadu_si128 ((__m128i*)((uint32_t*) (&image_src_info.raster[CURR_POS])));
            __m128i BackgrL = _mm_loadu_si128 ((__m128i*)((uint32_t*) (&image_dest_info.raster[CURR_POS])));
        
        // Step 2 - Separating bytes on halfs to gain more space for multiplication
            __m128i ForegrH = (__m128i) _mm_movehl_ps ((__m128) _m_zero, (__m128) ForegrL);
            __m128i BackgrH = (__m128i) _mm_movehl_ps ((__m128) _m_zero, (__m128) BackgrL);
        
        // Step 3 - Preparing bytes for multiplication by setting 1 byte space between 'em
            ForegrL  = _mm_cvtepi8_epi16 (ForegrL);
            ForegrH  = _mm_cvtepi8_epi16 (ForegrH);

            BackgrL  = _mm_cvtepi8_epi16 (BackgrL);
            BackgrH  = _mm_cvtepi8_epi16 (BackgrH);
        
        // Step 4 - shuffling the alpha parametr
            __m128i shuffle_mask = _mm_set_epi8 (0x80, 14, 0x80, 14, 0x80, 14, 0x80, 14,
                                                 0x80,  6, 0x80,  6, 0x80,  6, 0x80,  6);

            __m128i AlphaL  = _mm_shuffle_epi8 (ForegrL, shuffle_mask);
            __m128i AlphaH  = _mm_shuffle_epi8 (ForegrH, shuffle_mask);
        
        // Step 5 - Multiplying on alpha
            ForegrL  = _mm_mullo_epi16 (ForegrL, AlphaL);    
            ForegrH  = _mm_mullo_epi16 (ForegrH, AlphaH);   

            BackgrL  = _mm_mullo_epi16 (BackgrL,  _mm_sub_epi16 (_mm_set1_epi16(255), AlphaL));    
            BackgrH  = _mm_mullo_epi16 (BackgrH, _mm_sub_epi16 (_mm_set1_epi16(255), AlphaH));  

            __m128i SumLow  = _mm_add_epi16 (ForegrL, BackgrL);     
            __m128i SumHigh = _mm_add_epi16 (ForegrH, BackgrH);   
        
        // Step 6 - accumulation of results
            shuffle_mask = _mm_set_epi8 (0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
                                         15U,  13U,  11U,  9U,   7U,   5U,   3U,   1U);

            SumLow  = _mm_shuffle_epi8 (SumLow,  shuffle_mask);
            SumHigh = _mm_shuffle_epi8 (SumHigh, shuffle_mask);

        // Step 7 - Storing the result in SFML pixel array to draw it on the screen
            __m128i result = (__m128i) _mm_movelh_ps ((__m128) SumLow, (__m128) SumHigh);

            _mm_storeu_si128 ((__m128i*) (tmp_raster), result);

            color = sf::Color{tmp_raster[0]}; 
            image.setPixel (x, y, color); 
            color = sf::Color{tmp_raster[1]}; 
            image.setPixel (x + 1, y, color); 
            color = sf::Color{tmp_raster[2]}; 
            image.setPixel (x + 2, y, color); 
            color = sf::Color{tmp_raster[3]}; 
            image.setPixel (x + 3, y, color); 
        }
    } 

    return image;
}

#undef CURR_POS
#undef CUR_DST
#undef CUR_SRC

//-----------------------------------------------------------------------------

uint32_t find_min (uint32_t a1, uint32_t a2)
{
    if(a1 < a2)
    {
        return a1;
    }

    return a2;
}

//-----------------------------------------------------------------------------