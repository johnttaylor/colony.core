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

#include "Base.h"
#include "Cpl/System/FatalError.h"


///
using namespace Rte::Element;


////////////////////////
Base::Base( int   myDataType,
            bool  inUse,
            bool  validFlag
           )
:m_myDataType(myDataType)
.m_valid(validFlag)
,m_inUse(inUse)
    {
    }


////////////////////////
void Base::assertTypeMatches( const Api& other ) const
    {
    if ( m_myDataType.isNotEqual(other.getType()) )
        {
        Cpl::System::FatalError::logf( "Rte::Element::Base - type mismatch (me=%d, other=%d)", m_myDataType, other.getType()._dataTypeId );
        }
    }


////////////////////////
const DataType Base::getType(void) const
    {
    return m_myDataType;
    }

bool Base::isValid(void) const
    {
    return m_valid;
    }

void Base::setValidFlagState( bool newState )
    {
    m_valid = newState;
    }


////////////////////////
bool Base::isInUse(void) const
    {
    return m_inUse;
    }

void Base::setInUseState( bool newState )
    {
    m_inUse = newState;
    }




