#include "alpha_blending.h"
#include "image_utils.h"

//-----------------------------------------------------------------------------

int main (int argc, char const *argv[])
{
    sf::Clock clock;

    Image_info cat_image_info   = { 0 };
    Image_info table_image_info = { 0 };

    Image_info res_image_info   = { 0 };
    image_info_ctor (&res_image_info, 800, 600);

    FILE *file_cat   = fopen ("AskhatCat.bmp", "rb");
    FILE *file_table = fopen ("Table(1).bmp",  "rb");

    //Load images from bmp files:
    cat_image_info   = read_image (file_cat,   800, 600, 300, 220);
    table_image_info = read_image (file_table, 800, 600, 0,   0  );
    
    sf::RenderWindow window (sf::VideoMode (800, 600), "SUPER ALPHA BLENDING"); // <- Name
    
    while(window.isOpen ())                                 // Do while window is open
    {
        clock.restart();

        sf::Event event;                                    
        sf::Image image;

        while(window.pollEvent (event))                     // Handle closing the window
        {                                                   //
            if(event.type == sf::Event::Closed)             //
            {                                               //
                window.close ();                            //
            }                                               //
        }   

        for(int i = 0; i < N(TESTS); i++)                   // Blending
        {                                                   //
            alpha_blending_fast (&res_image_info,           //
                                  table_image_info,         //
                                  cat_image_info   );       //
        }  

        image = set_image (res_image_info);

        sf::Time elapsed_time = clock.getElapsedTime ();    // Iterations per second
        printf ("Iterations per second: %f\n", N(TESTS) / elapsed_time.asSeconds ());     

        sf::Texture texture;                                // Set texture
        texture.loadFromImage(image);                       //
        sf::Sprite sprite;                  

        sprite.setTexture(texture);                         //
        window.clear ();                                    //
        window.draw (sprite);                               //
        window.display ();                                  //                                                 
    }
    
    //bmp_dump (file_cat);
    create_bmp_file (res_image_info);

    FILE *res_file = fopen ("result.bmp", "rb");
    bmp_dump (res_file);
    fclose (res_file);

    fclose (file_table);
    fclose (file_cat);

    image_info_dtor (&cat_image_info);
    image_info_dtor (&table_image_info);
    image_info_dtor (&res_image_info);

    return 0;
}

//-----------------------------------------------------------------------------