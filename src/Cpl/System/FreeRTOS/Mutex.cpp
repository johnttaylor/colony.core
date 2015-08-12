/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Cpl/System/Mutex.h"
#include "Cpl/System/FatalError.h"



//////////////////////////////////////////////////////////////////////////////
Cpl::System::Mutex::Mutex()
    {
    m_mutex = xSemaphoreCreateRecursiveMutex();
    if ( m_mutex == NULL )
        {
        Cpl::System::FatalError::logf( "Cpl:System::Mutex::Mutex().  Failed to create mutex" );
        }
    }

Cpl::System::Mutex::~Mutex()
    {
    vSemaphoreDelete( m_mutex );
    }


void Cpl::System::Mutex::lock(void)    
    { 
    xSemaphoreTakeRecursive( m_mutex, portMAX_DELAY );
    }


void Cpl::System::Mutex::unlock(void)  
    { 
    xSemaphoreGiveRecursive( m_mutex );
    }
