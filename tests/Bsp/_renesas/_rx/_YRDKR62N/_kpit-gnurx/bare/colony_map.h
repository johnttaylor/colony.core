#ifndef COLONY_MAP_H_
#define COLONY_MAP_H_

// Timer vectors
#define BSP_VECTOR_CMTU0_CMI0    irqTimer1
#define BSP_VECTOR_CMTU1_CMI3    irqTimer2

extern void irqTimer1 (void) __attribute__ ((interrupt));
extern void irqTimer2 (void) __attribute__ ((interrupt));


// My BSP
#include "Bsp/_renesas/_rx/_YRDKR62N/_kpit_gnurx/Api.h"


//// strapi mapping
//#include "Cpl/Text/_mappings/mingw/strapi.h"


#endif
