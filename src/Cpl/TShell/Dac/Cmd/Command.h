#ifndef Cpl_TShell_Dac_Cmd_Command_h_
#define Cpl_TShell_Dac_Cmd_Command_h_
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

#include "colony_config.h"
#include "Cpl/TShell/Dac/Command.h"
#include "Cpl/TShell/Dac/Context_.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Text/String.h"


/** This symbol defines the default <esc> character for expanding text
 */
#ifndef OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_
#define OPTION_CPL_TSHELL_DAC_CMD_VAR_ESCAPE_CHAR_     "$"
#endif




///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {

/** This partially concrete class implements infrastructure and/or common 
    functionality for a DAC Shell command.
 */
class Command: public Cpl::TShell::Dac::Command
{
protected:
    /// Result when processing a conditional statement
    enum CondResult_T { eFALSE, //!< The conditional statement evaluated to false
                        eTRUE,  //!< The conditional statement evaluated to true
                        eERROR  //!< Error occuring during the evaulation of the conditional
                      }; 

protected:
    /// Command 
    Cpl::Container::KeyLiteralString  m_mapKey;


protected:
    /// Constructor
    Command( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb ) throw();

    /// Constructor. Used when creating a static instance of a command
    Command( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw();


public:
    /// See Cpl::TShell::Dac::Command
    const char* getVerb() const throw();


protected:
    /// See Cpl::Container::Key
    const Cpl::Container::Key& getKey() const throw();


protected:
    /// Helper method to expand text
    static Result_T expandText( const char* textToExpand, Cpl::Text::String& dst, Cpl::TShell::Dac::ActiveVariablesApi& vars ) throw();

    /// Helper method that processes the a conditional expression
    static CondResult_T conditional( Cpl::TShell::Dac::Context_&           context, 
                                     Cpl::Text::Tokenizer::TextBlock&      tokens, 
                                     unsigned                              startingTokenIndex, 
                                     Cpl::TShell::Dac::ActiveVariablesApi& vars 
                                   ) throw();

    /// Helper method that compares to values
    static CondResult_T evaluate( const char* leftVal, const char* oper, const char* rightVal ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
