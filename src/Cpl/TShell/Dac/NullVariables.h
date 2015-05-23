#ifndef Cpl_TShell_Dac_NullVariables_h_
#define Cpl_TShell_Dac_NullVariables_h_
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

#include "Cpl/TShell/Dac/ActiveVariablesApi.h"

///
namespace Cpl { namespace TShell { namespace Dac {

/** This concrete class implements the Active DAC Variables interface with ZERO 
    supported USER variables.  This is typically used when the Application does 
    not need/want shell variable support.  
 */
class NullVariables: public ActiveVariablesApi
{
public:
    /// Constructor
    NullVariables();


public:
    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* get( const Cpl::Container::Key& variableName ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* find( const Cpl::Container::Key& variableName ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    void remove( VariableApi& varNoLongerInUse ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    unsigned getUserCount() const throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    unsigned getMaxUserCount() const throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    void addSystem( VariableApi& systemVariable ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* first() const throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* next( VariableApi& current ) const throw();
};


};      // end namespaces
};
};
#endif  // end header latch
