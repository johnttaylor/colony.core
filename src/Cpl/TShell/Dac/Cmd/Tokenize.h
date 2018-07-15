#ifndef Cpl_TShell_Dac_Cmd_Tokenize_h
#define Cpl_TShell_Dac_Cmd_Tokenize_h
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
#include "Cpl/TShell/Dac/Cmd/Command.h"


/** Usage
                                         "         1         2         3         4         5         6         7         8"
                                         "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define CPLTSHELLDACMD_USAGE_TOKENIZE_   "tokenize *|<dqe> <etext> <var1> <idx1> [<var2> <idx2>]..."

/// Detailed Help text
#ifndef CPLTSHELLDACMD_DETAIL_TOKENIZE_
#define CPLTSHELLDACMD_DETAIL_TOKENIZE_  "  Tokenizes the contents of <etext> and stores the token(s) in specified <varN>\n" \
                                         "  variables based on the associated <idxN> arugments - which is a zero based\n" \
                                         "  index of the token being stored. The <dqe> argument specifies the delimeter,\n" \
                                         "  quote, and escape characters (respectively) to be used during the tokenize\n" \
                                         "  operation. If '*' is specified in lieu of the <dqe> argument then: space,\n" \
                                         "  quote, back-tick characters are used for the vaule of <dqe>.  See the Set\n" \
                                         "  command for additional details about 'etext' arugments."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class Tokenize: public Command
{
public:
    /// See Cpl::TShell::Dac::Command
    const char* getUsage() const throw()    { return CPLTSHELLDACMD_USAGE_TOKENIZE_; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const throw()    { return CPLTSHELLDACMD_DETAIL_TOKENIZE_; }
    
     
public:
    /// Constructor
    Tokenize( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList ) throw();

    /// Constructor.  Used to create a static instance of the command
    Tokenize( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw();


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
