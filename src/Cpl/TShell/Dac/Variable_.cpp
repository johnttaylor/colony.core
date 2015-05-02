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

#include "Variable_.h"
#include "Cpl/Text/atob.h"


///
using namespace Cpl::TShell::Dac;


/////////////////////////////////////
Variable_::Variable_( const char* name )
:m_value(""),
 m_name(name)
    {
    }



/////////////////////////////////////
int Variable_::compare( const char* leftValue, const char* rightValue )
    {
    double leftOper  = 0.0;
    double rightOper = 0.0;

    // Compare as numeric
    if ( Cpl::Text::a2d( leftOper, leftValue ) && Cpl::Text::a2d( rightOper, rightValue ) )
        {
        if ( leftOper == rightOper )
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
const char* Variable_::getName() const throw()
    {
    return m_name;
    }

const char* Variable_::getValue() const throw()
    {
    return m_value;
    }


/////////////////////////////////////
bool Variable_::setValue( const char* newValue ) throw()
    {
    m_value = newValue;
    }

bool Variable_::setValue( double newValue ) throw()
    {
    m_value.format( "%f", newValue ); 
    }

bool Variable_::setValue( long newValue ) throw()
    {
    m_value = newValue;
    }

bool Variable_::setValue( unsigned long newValue ) throw()
    {
    m_value = newValue;
    }


/////////////////////////////////////
bool Variable_::add( const char* amount )
    {
    double leftOper  = 0.0;
    double rightOper = 0.0;

    // Add as numeric
    if ( Cpl::Text::a2d( leftOper, leftValue ) && Cpl::Text::a2d( rightOper, rightValue ) )
        {
        setValue( leftOper + rightOper );
        return true;
        }

    return false;
    }


/////////////////////////////////////
bool Variable_::getNumber( double& valueAsNumber ) const throw()
    {
    return Cpl::Text::a2d( valueAsNumber, m_value );
    }

bool Variable_::getNumber( long& valueAsNumber ) const throw()
    {
    double value  = 0.0;
    bool   rc     = Cpl::Text::a2d( value, m_value );
    valueAsNumber = (long) value;
    return rc;
    }

bool Variable_::getNumber( unsigned long& valueAsNumber ) const throw()
    {
    double value  = 0.0;
    bool   rc     = Cpl::Text::a2d( value, m_value );
    valueAsNumber = (unsigned long) value;
    return rc;
    }

