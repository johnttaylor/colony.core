/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2018  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "Precision.h"
#include <math.h>
#include "Cpl/Text/atob.h"



///
using namespace Rte::Element;

static const Cpl::System::ElapsedTime::Precision_T zero_ = {0};

/////////////////////////////
ElapsedPrecisionTime::ElapsedPrecisionTime( bool    inUse,
                      int8_t  validState
                    )
:Basic<Cpl::System::ElapsedTime::Precision_T,DataType::PRECISION_T>( zero_, inUse, validState )
    {
    m_data.m_seconds     = 0;
    m_data.m_thousandths = 0;
    }


const char* ElapsedPrecisionTime::getTypeAsText(void) const
    {
    return "PRECISION_T";
    }


const char* ElapsedPrecisionTime::toString( Cpl::Text::String& dstMemory, bool append ) const
    {
    if ( convertStateToText( dstMemory, append ) )
        {
        dstMemory.formatOpt( append, "%ul.%ud", m_data.m_seconds, m_data.m_thousandths );
        }

    return dstMemory;
    }


const char* ElapsedPrecisionTime::setFromText( const char* srcText, const char* terminationChars )
    {
    const char* endPtr = 0;
    double      temp;
    if ( Cpl::Text::a2d(temp, srcText, terminationChars, &endPtr) )
        {
        if ( !isModelElement() || !isLocked() )
            {
            double intpart       = 0.0;
            double fracpart      = modf( temp, &intpart );
            m_data.m_seconds     = (unsigned long) intpart;
            m_data.m_thousandths = (uint16_t)(fracpart * 1000);
            }

        return endPtr;
        }

    return 0;
    }


