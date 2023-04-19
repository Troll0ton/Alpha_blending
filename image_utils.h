//! @file image_utils.h

#ifndef IMAGE_UTILS
#define IMAGE_UTILS

//-----------------------------------------------------------------------------

#include "common.h"

//-----------------------------------------------------------------------------

//need alignment
#pragma pack(push, 2)
typedef struct Bitmap_file_header
{
    byte_2 bfType;
    byte_4 bfSize;
    byte_2 bfReserved1;
    byte_2 bfReserved2;
    byte_4 bfOffBits;
} Bitmap_file_header;
#pragma pack(pop)

//-----------------------------------------------------------------------------

//need alignment
#pragma pack(push, 2)
typedef struct Bitmap_file_header_info
{
    byte_4 biSize;
    byte_4 biWidth;
    byte_4 biHeight;
    byte_2 biPlanes;
    byte_2 biBitCount;
    byte_4 biCompression;
    byte_4 biSizeImage;
    byte_4 biXPelsPerMeter;
    byte_4 biYPelsPerMeter;
    byte_4 biClrUsed;
    byte_4 biClrImportant;
} Bitmap_file_header_info;
#pragma pack(pop)

//-----------------------------------------------------------------------------

Image_info read_image (FILE    *image_file,  
                       uint32_t width,    uint32_t height, 
                       uint32_t pos_x,    uint32_t pos_y  );
                       
sf::Image set_image (Image_info image_info);

void image_info_ctor (Image_info *image_info, uint32_t width, 
                                              uint32_t height);

void image_info_dtor (Image_info *image_info);

void bmp_dump (FILE *image_file);

void create_bmp_file (Image_info image_info);

//-----------------------------------------------------------------------------

#endif //IMAGE_UTILS