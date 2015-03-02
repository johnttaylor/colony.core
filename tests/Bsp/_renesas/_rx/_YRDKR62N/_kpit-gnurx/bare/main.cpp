#include "Bsp/Api.h"



int main(void)
    {
    // Initialize the board
    Bsp_Api_initialize();

    // Spin the LEDs
    for(;;)
        {
        uint8_t m;
        int     i;
        for( m = 4; m <= 12; m++ )
            {
            Bsp_toggleCircleLED( m );

            for(i=0; i< 100000; i++)
                {
                uint8_t n;
                for( n=1; n <= 3; n++ )
                    {
                    if ( Bsp_isButtonPressed( n ) )
                        {
                        Bsp_setCircleLED( 12+n, true );
                        }
                    else
                        {
                        Bsp_setCircleLED( 12+n, false );
                        }
                    }
                }
            }
        }
        
    return 0;
    }
