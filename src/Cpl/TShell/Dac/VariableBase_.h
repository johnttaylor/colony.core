#ifndef Cpl_TShell_Dac_VariableBase_x_h_
#define Cpl_TShell_Dac_VariableBase_x_h_
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

#include "Cpl/TShell/Dac/VariableApi.h"
#include "Cpl/Text/FString.h"



///
namespace Cpl { namespace TShell { namespace Dac {

/** This Private Namespace concrete class provides base implementation for
    a DAC shell variable. The memory for the shell's value MUST be provided
    by a child class.
 */

class VariableBase_: public VariableApi
{
protected:
    /// Value buffer
    Cpl::Text::String& m_value;

    /// Name buffer
    Cpl::Text::FString<OPTION_CPL_TSHELL_DAC_VARIABLEAPI_NAME_SIZE> m_name;


public:
    /// Constructor
    VariableBase_( const char* name, Cpl::Text::String& storageForValue );


public:
    /// See Cpl::Container::Key
    const Cpl::Container::Key& getKey() const throw() { return m_name; }


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
    bool add( const char* amount ) throw();

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
