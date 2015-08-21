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

#include "Cpl/System/FastLock.h"


//////////////////////////////////////////////////////////////////////////////
Cpl::System::FastLock::FastLock()
    {
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr,PTHREAD_MUTEX_DEFAULT);
    pthread_mutex_init(&m_flock, &mutex_attr);
    }

Cpl::System::FastLock::~FastLock()
    {
    // Guarantee that the mutex is unlocked before it is "destroyed"
    pthread_mutex_unlock(&m_flock);  
    pthread_mutex_destroy(&m_flock);
    }

void Cpl::System::FastLock::lock(void)    
    { 
    pthread_mutex_lock(&m_flock);
    }


void Cpl::System::FastLock::unlock(void)  
    { 
    pthread_mutex_unlock(&m_flock);
    }
