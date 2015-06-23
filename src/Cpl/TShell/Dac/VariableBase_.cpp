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

#include "VariableBase_.h"
#include "Cpl/Text/atob.h"
#include <math.h>
#include "Cpl/Math/real.h"


///
using namespace Cpl::TShell::Dac;


/////////////////////////////////////
VariableBase_::VariableBase_( const char* name, Cpl::Text::String& storageForValue )
:m_value(storageForValue),
 m_name(name)
    {
    }



/////////////////////////////////////
int VariableApi::compare( const char* leftValue, const char* rightValue )
    {
    double leftOper  = 0.0;
    double rightOper = 0.0;

    // Compare as numeric
    if ( Cpl::Text::a2d( leftOper, leftValue ) && Cpl::Text::a2d( rightOper, rightValue ) )
        {
        if ( Cpl::Math::areDoublesEqual(leftOper,rightOper) )
            {
            return 0;
            }
        else if ( leftOper < rightOper )
            {
            return -1;
            }
        else
            {
            return 1;
            }
        }

    // Compare as a string
    return strcmp( leftValue, rightValue );
    }      

/////////////////////////////////////
const char* VariableBase_::getName() const throw()
    {
    return m_name;
    }

const char* VariableBase_::getValue() const throw()
    {
    return m_value;
    }


/////////////////////////////////////
bool VariableBase_::setValue( const char* newValue ) throw()
    {
    m_value = newValue;
    return m_value.truncated() == false;
    }

bool VariableBase_::setValue( double newValue ) throw()
    {
    m_value.format( "%f", newValue ); 
    return m_value.truncated() == false;
    }

bool VariableBase_::setValue( long newValue ) throw()
    {
    m_value = newValue;
    return m_value.truncated() == false;
    }

bool VariableBase_::setValue( unsigned long newValue ) throw()
    {
    m_value = newValue;
    return m_value.truncated() == false;
    }

bool VariableBase_::setNumericValue( double newValue ) throw()
    {
    double intpart;
    double fracpart = modf( newValue, &intpart );

    if ( Cpl::Math::areDoublesEqual(fracpart,0) )
        {
        return setValue( newValue );
        }
    else
        {
        return setValue( (long) intpart );
        }
    }


/////////////////////////////////////
bool VariableBase_::add( const char* amount ) throw()
    {
    double leftOper  = 0.0;
    double rightOper = 0.0;

    // Add as numeric
    if ( Cpl::Text::a2d( leftOper, m_value ) && Cpl::Text::a2d( rightOper, amount ) )
        {
        setNumericValue( leftOper + rightOper );
        return true;
        }

    return false;
    }


/////////////////////////////////////
bool VariableBase_::getNumber( double& valueAsNumber ) const throw()
    {
    return Cpl::Text::a2d( valueAsNumber, m_value );
    }

bool VariableBase_::getNumber( long& valueAsNumber ) const throw()
    {
    double value  = 0.0;
    bool   rc     = Cpl::Text::a2d( value, m_value );
    valueAsNumber = (long) value;
    return rc;
    }

bool VariableBase_::getNumber( unsigned long& valueAsNumber ) const throw()
    {
    double value  = 0.0;
    bool   rc     = Cpl::Text::a2d( value, m_value );
    valueAsNumber = (unsigned long) value;
    return rc;
    }

