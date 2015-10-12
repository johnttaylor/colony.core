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


#include "ElaspedTime.h"

///
using namespace Cpl::System;


#define MAX_POSITVE_MSEC_   0x7FFF


///////////////////////////////////////////////////////
ElaspedTime::Precision_T ElaspedTime::deltaPrecision( Precision_T startTime, Precision_T endTime ) throw()
    {
    Precision_T delta;

    // Calc delta seconds
    delta.m_seconds = endTime.m_seconds - startTime.m_seconds;

    // Calc delta milliseoncds (trap case of 'borrow')
    delta.m_thousandths = endTime.m_thousandths - startTime.m_thousandths;
    if ( delta.m_thousandths > MAX_POSITVE_MSEC_ )
        {
        delta.m_seconds--;
        delta.m_thousandths += 1000;
        }

    return delta;
    }


bool ElaspedTime::expiredPrecision( Precision_T timeMarker, Precision_T duration, Precision_T currentTime ) throw()
    {
    Precision_T delta = ElaspedTime::deltaPrecision( timeMarker, currentTime );
    return delta.m_seconds >= duration.m_seconds && delta.m_thousandths >= duration.m_thousandths;
    }
