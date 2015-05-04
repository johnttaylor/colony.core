#ifndef Cpl_TShell_Dac_Variable_x_h_
#define Cpl_TShell_Dac_Variable_x_h_
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
/** @file */

#include "colony_config.h"
#include "Cpl/TShell/Dac/VariableApi.h"
#include "Cpl/Text/FString.h"


/** This symbol defines the size of the string (not including the null 
    terminator) used to store a Variable's name.
 */
#ifndef OPTION_CPL_TSHELL_DAC_VARIABLE_NAME_SIZE
#define OPTION_CPL_TSHELL_DAC_VARIABLE_NAME_SIZE    16
#endif

/** This symbol defines the size of the string (not including the null 
    terminator) used to store a Variable's value.
 */
#ifndef OPTION_CPL_TSHELL_DAC_VARIABLE_VALUE_SIZE
#define OPTION_CPL_TSHELL_DAC_VARIABLE_VALUE_SIZE   64
#endif


///
namespace Cpl { namespace TShell { namespace Dac {

/** This Private Namespace concrete class implements a DAC Shell variable
 */

class Variable_: public VariableApi
{
protected:
    /// Value buffer
    Cpl::Text::FString<OPTION_CPL_TSHELL_DAC_VARIABLE_VALUE_SIZE>   m_value;

    /// Name buffer
    Cpl::Text::FString<OPTION_CPL_TSHELL_DAC_VARIABLE_NAME_SIZE>    m_name;


public:
    /// Constructor
    Variable_( const char* name );




public:
    /// See Cpl::Container::Key
    const Cpl::Container::Key& getKey() const throw() { return m_name; }


public:
    /// See Cpl::TShell::Dac::VariableApi
    static int compare( const char* leftValue, const char* rightValue );


public:
    /// See Cpl::TShell::Dac::VariableApi
    const char* getName() const throw();

    /// See Cpl::TShell::Dac::VariableApi
    const char* getValue() const throw();

    /// See Cpl::TShell::Dac::VariableApi
    bool setValue( const char* newValue ) throw();

    /// See Cpl::TShell::Dac::VariableApi
    bool setValue( double newValue ) throw();

    /// See Cpl::TShell::Dac::VariableApi
    bool setValue( long newValue ) throw();

    /// See Cpl::TShell::Dac::VariableApi
    bool setValue( unsigned long newValue ) throw();

    /// See Cpl::TShell::Dac::VariableApi
    bool add( const char* amount );

    /// See Cpl::TShell::Dac::VariableApi
    bool getNumber( double& valueAsNumber ) const throw();

    /// See Cpl::TShell::Dac::VariableApi
    bool getNumber( long& valueAsNumber ) const throw();

    /// See Cpl::TShell::Dac::VariableApi
    bool getNumber( unsigned long& valueAsNumber ) const throw();
};

};      // end namespaces
};
};
#endif  // end header latch
