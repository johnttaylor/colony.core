#ifndef Cpl_TShell_Dac_Variable_x_h_
#define Cpl_TShell_Dac_Variable_x_h_
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
/** @file */

#include "Cpl/TShell/Dac/VariableBase_.h"



///
namespace Cpl { namespace TShell { namespace Dac {

/** This Private Namespace concrete class implements a DAC Shell variable
    a DAC shell variable. The memory for the shell's value MUST be provided
    by a child class.
 */

class Variable_: public VariableBase_
{
protected:
    /// Value buffer
    Cpl::Text::FString<OPTION_CPL_TSHELL_DAC_VARIABLEAPI_VALUE_SIZE>    m_storageForValue;


public:
    /// Constructor
    Variable_( const char* name ):VariableBase_(name,m_storageForValue),m_storageForValue(""){}

};

};      // end namespaces
};
};
#endif  // end header latch
