#include "image_utils.h"

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

Image_info read_image (FILE    *image_file,  
                       uint32_t width,    uint32_t height, 
                       uint32_t pos_x,    uint32_t pos_y  )
{
    Image_info image_info;

    //              bin map of file header
    //                       v
    Bitmap_file_header      bfh      = { 0 };
    Bitmap_file_header_info bfh_info = { 0 };

    //Read bitmap_file_header
    fread ((void*)(&bfh), sizeof (char), FH_SIZE, image_file);
    //Read bitmap_file_header_info
    fread ((void*)(&bfh_info), sizeof (char), FH_INFO_SIZE, image_file);

    //Skip scheme and bitmaps
    fseek (image_file, bfh.bfOffBits, SEEK_SET);

    INFO(width)  = width; 
    INFO(height) = height; 
    INFO(size)   = width * height; 

    //Read Array
    INFO(raster) = (Pix_color*) calloc (INFO(size), RGBA);

    Pix_color *curr_pos = INFO(raster) + ((pos_y + bfh_info.biHeight - 1) * width + pos_x);

    for(int i = 0; i < bfh_info.biHeight; i++)
    {
        fread (curr_pos, RGBA, bfh_info.biWidth, image_file);

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

void bmp_dump (FILE *image_file)
{
    // save curr pos in file
    fpos_t curr_pos;
    fgetpos (image_file, &curr_pos);

    // move to begin
    fseek (image_file, 0, SEEK_SET);

    Bitmap_file_header      bfh      = { 0 };
    Bitmap_file_header_info bfh_info = { 0 };

    //Read bitmap_file_header
    fread ((void*)(&bfh), sizeof (char), FH_SIZE, image_file);
    //Read bitmap_file_header_info
    fread ((void*)(&bfh_info), sizeof (char), FH_INFO_SIZE, image_file);

    //DUMP
    printf ("_______________________________________ BMP FILE _______________________________________\n\n"
            "(bitmap_file_header)\n"
            "bfType - %hu\n"
            "bfSize - %u\n"
            "bfReserved1 - %hu\n"
            "bfReserved2 - %hu\n"
            "bfOffBits - %u\n\n"
            "(bitmap_file_header_info)\n"
            "biSize - %u\n"
            "biWidth - %u\n"
            "biHeight - %u\n"
            "biPlanes - %hu\n"
            "biBitCount - %hu\n"
            "biCompression - %u\n"
            "biSizeImage - %u\n"
            "biXPelsPerMeter - %u\n"
            "biYPelsPerMeter - %u\n"
            "biClrUsed - %u\n"
            "biClrImportant - %u\n"
            "________________________________________________________________________________________\n\n",
            bfh.bfType, bfh.bfSize, bfh.bfReserved1, bfh.bfReserved2, bfh.bfOffBits,
            bfh_info.biSize, bfh_info.biWidth, bfh_info.biHeight, bfh_info.biPlanes, bfh_info.biBitCount,
            bfh_info.biCompression, bfh_info.biSizeImage, bfh_info.biXPelsPerMeter, 
            bfh_info.biYPelsPerMeter, bfh_info.biClrUsed, bfh_info.biClrImportant
            );

    // return to same pos
    fsetpos (image_file, &curr_pos);
}

//-----------------------------------------------------------------------------

void create_bmp_file (Image_info image_info)
{
    FILE *image_file = fopen ("result.bmp", "wb");

    Bitmap_file_header      bfh      = { 0 };
    Bitmap_file_header_info bfh_info = { 0 };

    bfh.bfType = 19778;                     //BMP FORMAT
    bfh.bfSize = image_info.size * 4 + FH_SIZE + FH_INFO_SIZE;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = FH_SIZE + FH_INFO_SIZE;

    bfh_info.biSize = FH_INFO_SIZE;
    bfh_info.biWidth = image_info.width;
    bfh_info.biHeight = image_info.height;
    bfh_info.biPlanes = 1;
    bfh_info.biBitCount = 32;
    bfh_info.biCompression = 0;
    bfh_info.biSizeImage = image_info.size * 4;
    bfh_info.biXPelsPerMeter = 2834;
    bfh_info.biYPelsPerMeter = 2834;
    bfh_info.biClrUsed = 0;
    bfh_info.biClrImportant = 0;

    fwrite (&bfh,      FH_SIZE,      1, image_file);
    fwrite (&bfh_info, FH_INFO_SIZE, 1, image_file);

    Pix_color *curr_pos = INFO(raster) + ((image_info.height - 1) * image_info.width);

    for(int i = 0; i < image_info.height; i++)
    {
        fwrite (curr_pos, RGBA, image_info.width, image_file);

        curr_pos -= image_info.width;
    }

    fclose (image_file);
}

//-----------------------------------------------------------------------------

void image_info_ctor (Image_info *image_info, uint32_t width, 
                                              uint32_t height)
{
    image_info->size = width * height;
    image_info->width = width;
    image_info->height = height;

    image_info->raster = (Pix_color*) calloc (image_info->size, RGBA);
}

//-----------------------------------------------------------------------------

void image_info_dtor (Image_info *image_info)
{
    image_info->height = DELETED_PAR;
    image_info->width  = DELETED_PAR;
    image_info->size   = DELETED_PAR;

    free (image_info->raster);
}

//-----------------------------------------------------------------------------