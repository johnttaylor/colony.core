#ifndef Rte_TShell_Dac_Cmd_RteRead_h
#define Rte_TShell_Dac_Cmd_RteRead_h
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
#include "Rte/TShell/Dac/Cmd/Command.h"


/** Usage
                                        "         1         2         3         4         5         6         7         8"
                                        "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define RTETSHELLDACMD_USAGE_TRTEREAD_  "rte-read ls|ll [<filter>]\n" \
                                        "rte-read <point>\n" \
                                        "rte-read <point> <idx>"

/// Detailed Help text
#ifndef RTETSHELLDACMD_DETAIL_TRTEREAD_
#define RTETSHELLDACMD_DETAIL_TRTEREAD_ "  Reads a Point or single Tuple within a RTE Model point.  <point> is the\n" \
                                        "  symbolic name of a RTE Model Point and <idx> is the zero base index of the\n" \
                                        "  Tuple to read.  When 'ls|ll' is used, a list of Model points is generated. The\n" \
                                        "  <filter> argument will only list points that contain <filter>. Note: <etext>\n" \
                                        "  is support for all parameters."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



///
namespace Rte { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class RteRead: public Command
{
public:
    /// See Cpl::TShell::Dac::Command
    const char* getUsage() const throw()   { return RTETSHELLDACMD_USAGE_TRTEREAD_; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const throw()    { return RTETSHELLDACMD_DETAIL_TRTEREAD_; }
    
     
public:
    /// Constructor
    RteRead( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList ) throw();

    /// Constructor.  Used to create a static instance of the command
    RteRead( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw();


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
