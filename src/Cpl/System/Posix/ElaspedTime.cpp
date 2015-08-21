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

#include "Cpl/System/ElaspedTime.h"
#include "Cpl/System/Private_.h"
#include <stdlib.h>
#include <time.h>


/// 
using namespace Cpl::System;

static unsigned long getTime_()
    {
    struct timespec tm;
    clock_gettime(CLOCK_MONOTONIC, &tm );
    return tm.tv_sec * 1000 + tm.tv_nsec / 1000000;
    }

///////////////////////////////////////////////////////////////
static unsigned long elaspedMsec_;
static unsigned long lastMsec_;

namespace {

    /// This class is to 'zero' the elasped to the start of the application
    class RegisterInitHandler_: public Cpl::System::StartupHook_
    {                               
    public:
        ///
        RegisterInitHandler_():StartupHook_(eSYSTEM) {}

            
    protected:
        ///
        void notify( InitLevel_T init_level )
            {
            elaspedMsec_  = 0;
            lastMsec_     = getTime_();
            }

    };
}; // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


///////////////////////////////////////////////////////////////
unsigned long ElaspedTime::millisecondsInRealTime( void ) throw()
    {
    Cpl::System::Mutex::ScopeBlock lock( Cpl::System::Locks_::system() );

    unsigned long newTime = getTime_();
    unsigned long delta   = newTime - lastMsec_;
    elaspedMsec_         += delta;
    lastMsec_             = newTime;

    return elaspedMsec_;
    }

unsigned long ElaspedTime::secondsInRealTime( void ) throw()
    {
    Cpl::System::Mutex::ScopeBlock lock( Cpl::System::Locks_::system() );

    // Update my internal elaspedMsec time
    milliseconds();

    // Convert my internal elasped time to seconds
    return (unsigned long)(elaspedMsec_ / 1000LL);
    }


ElaspedTime::Precision_T ElaspedTime::precisionInRealTime( void ) throw()
    {
    Cpl::System::Mutex::ScopeBlock lock( Cpl::System::Locks_::system() );

    // Update my internal elaspedMsec time
    milliseconds();

    // Convert to my Precision format
    Precision_T now;
    lldiv_t     result = lldiv(elaspedMsec_,1000LL);
    now.m_seconds      = (unsigned long) result.quot;
    now.m_thousandths  = (uint16_t)      result.rem;
    return now;
    }
