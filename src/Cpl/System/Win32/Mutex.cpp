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

#include "Cpl/System/Mutex.h"


//////////////////////////////////////////////////////////////////////////////
Cpl::System::Mutex::Mutex()
    {
    m_mutex = CreateMutex( 0, FALSE, 0 );
    }

Cpl::System::Mutex::~Mutex()
    {
    // Guarantee that the mutex is unlocked before it is released
    ReleaseMutex(m_mutex);
    CloseHandle(m_mutex);
    }

void Cpl::System::Mutex::lock(void)    
    { 
    WaitForSingleObject(m_mutex,INFINITE);
    }


void Cpl::System::Mutex::unlock(void)  
    { 
    ReleaseMutex(m_mutex);
    }


