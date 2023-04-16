#include "bmp_utils.h"

//-----------------------------------------------------------------------------
    //                             ATTENTION
    //  This function extend image to new sizes
    //  You need to set the position of old picture in new raster
    //  EXAMPLE:
    //   _________________________
    //  |                         |  
    //  |       _____             |  
    //  |      |     |            |
    //  |      |_____|            |
    //  |                         |
    //  |                         |
    //  | _______________________ |

Image_info read_image (FILE    *bmp_file,  
                       uint32_t width,    uint32_t height, 
                       uint32_t pos_x,    uint32_t pos_y  )
{
    Image_info image_info;

    //              bin map of file header
    //                       v
    Bitmap_file_header      bfh      = { 0 };
    Bitmap_file_header_info bfh_info = { 0 };

    //Read bitmap_file_header
    fread ((void*)(&bfh), sizeof (char), FH_SIZE, bmp_file);
    //Read bitmap_file_header_info
    fread ((void*)(&bfh_info), sizeof (char), FH_INFO_SIZE, bmp_file);

    INFO(width)  = width; 
    INFO(height) = height; 
    INFO(size)   = width * height; 

    //Read Array
    INFO(raster) = (Pix_color*) calloc (INFO(size), RGBA);

    Pix_color *curr_pos = INFO(raster) + ((pos_y + bfh_info.biHeight - 1) * width + pos_x);

    for(int i = 0; i < bfh_info.biHeight; i++)
    {
        fread (curr_pos, RGBA, bfh_info.biWidth, bmp_file);

        curr_pos -= width;
    }

    return image_info;
}

//-----------------------------------------------------------------------------

#define CURR_POS y * image_info.width + x

sf::Image set_image (Image_info image_info)
{     
    sf::Image image;

    image.create (image_info.width, image_info.height, sf::Color::Black);

    for(int y = 0; y < image_info.height; y++)                                                    
    {                                                                                   
        for(int x = 0; x < image_info.width; x++)                                      
        {                                                                               
            sf::Color color = sf::Color{image_info.raster[CURR_POS].R,               
                                        image_info.raster[CURR_POS].G,               
                                        image_info.raster[CURR_POS].B,               
                                        image_info.raster[CURR_POS].A };             
                                                                                        
            image.setPixel (x, y, color);                 
        }                                                                               
    }

    return image;
}

#undef CURR_POS

//-----------------------------------------------------------------------------

void image_info_dtor (Image_info *image_info)
{
    image_info->height = DELETED_PAR;
    image_info->width  = DELETED_PAR;
    image_info->size   = DELETED_PAR;

    free (image_info->raster);
}

//-----------------------------------------------------------------------------