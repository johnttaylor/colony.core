#include "Bsp/Api.h"

#define TIMER1_HZ_RATE      1000
#define TIMER2_HZ_RATE      1000


int main(void)
    {
    // Initialize the board
    Bsp_Api_initialize();
    Bsp_initCompareMatchTimer( 0, TIMER1_HZ_RATE, 0, 14 );  
    Bsp_initCompareMatchTimer( 3, TIMER2_HZ_RATE, 0, 14 );  
    Bsp_setCircleLED( 11, true );
    Bsp_setCircleLED( 12, false );

    // Enable interrupts
    Bsp_Api_enableIrqs();

    // Start my timers
    Bsp_startCompareMatchTimer( 0 );
    Bsp_startCompareMatchTimer( 3 );
    

    // Spin the LEDs
    for(;;)
        {
        uint8_t m;
        int     i;
        for( m = 4; m <= 10; m++ )
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



void irqTimer1( void )
    {
    static uint16_t counter = 0;
    static bool     oncycle = true;

    counter++;
    if ( oncycle )
        {
        if (  counter > TIMER1_HZ_RATE * 3 / 4 )
            {
            Bsp_setCircleLED( 11, false );
            oncycle = false;
            }
        }
    else if ( counter > TIMER1_HZ_RATE )
        {
        Bsp_setCircleLED( 11, true );
        counter = 0;
        oncycle = true;
        }
    }

void irqTimer2( void )
    {
    static uint16_t counter = 0;
    static bool     oncycle = false;

    counter++;
    if ( !oncycle )
        {
        if ( counter > TIMER2_HZ_RATE * 3 / 4 )
            {
            Bsp_setCircleLED( 12, true );
            oncycle = true;
            }
        }
    else if ( counter > TIMER1_HZ_RATE )
        {
        Bsp_setCircleLED( 12, false );
        counter = 0;
        oncycle = false;
        }
    }
