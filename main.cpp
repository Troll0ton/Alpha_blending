#include "alpha_blending.h"
#include "bmp_utils.h"

//-----------------------------------------------------------------------------

int main (int argc, char const *argv[])
{
    sf::Clock clock;

    Image_info cat_image_info;
    Image_info table_image_info;

    FILE *file_cat   = fopen ("AskhatCat.bmp", "rb");
    FILE *file_table = fopen ("Table(1).bmp",     "rb");

    //Load images from bmp files:
    cat_image_info   = read_image (file_cat,   800, 600, 300, 220);
    table_image_info = read_image (file_table, 800, 600, 0,   0  );

    sf::RenderWindow window (sf::VideoMode (800, 600), "SUPER ALPHA BLENDING"); // <- Name
    
    while(window.isOpen ())                                 //Do while window is open
    {
        clock.restart();

        sf::Event event;                                    //Handle all events

        while(window.pollEvent (event))                     //Handle closing the window
        {                                                   //
            if(event.type == sf::Event::Closed)             //
            {                                               //
                window.close ();                            //
            }                                               //
        }   

        sf::Image image = alpha_blending_fast (table_image_info, cat_image_info);  

        sf::Texture texture;                                //Set texture
        texture.loadFromImage(image);                       //
        sf::Sprite sprite;                  

        sprite.setTexture(texture);                         //
        window.clear ();                                    //
        window.draw (sprite);                               //
        window.display ();                                  //

        sf::Time elapsed_time = clock.getElapsedTime ();     //Find FPS
        //printf ("fps: %f\n", 1/elapsed_time.asSeconds ());   //                                                    
    }

    fclose (file_table);
    fclose (file_cat);

    image_info_dtor (&cat_image_info);
    image_info_dtor (&table_image_info);

    printf ("-------------FINISH-------------\n\n");

    return 0;
}

//-----------------------------------------------------------------------------