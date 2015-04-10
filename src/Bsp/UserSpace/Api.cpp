/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014, 2015  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Bsp/Api.h"
#include "Cpl/System/Mutex.h"


static Cpl::System::Mutex irqLock_;


///////////////////////////////////////////////////////////
void Bsp_Api_initialize( void )
    {
    }


void Bsp_Api_disableIrqs_MAP(void)
    {
    irqLock_.lock();
    }


void Bsp_Api_enableIrqs_MAP(void)
    {
    irqLock_.unlock();
    }


void Bsp_Api_pushAndDisableIrqs_MAP(void)
    {
    irqLock_.lock();
    }

                     
void Bsp_Api_popIrqs_MAP(void)
    {
    irqLock_.unlock();
    }


void Bsp_Api_nop_MAP(void)
    {
    }


