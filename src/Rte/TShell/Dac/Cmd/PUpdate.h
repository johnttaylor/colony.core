#ifndef Rte_TShell_Dac_Cmd_PUpdate_h
#define Rte_TShell_Dac_Cmd_PUpdate_h
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
#define RTETSHELLDACMD_USAGE_PUPDATE_  "pupdate ls|ll [<filter>]\n" \
                                       "pupdate <point> {<tuple0>[,<tuple1>]*}\n" \
                                       "pupdate <point> [!|^][?[<num>]{}" 

/// Detailed Help text
#ifndef RTETSHELLDACMD_DETAIL_PUPDATE_
#define RTETSHELLDACMD_DETAIL_PUPDATE_ "  Updates an entire RTE Model point.  <point> is the symbolic name of a RTE\n" \
                                       "  Model Point.  See the 'tupdate' command for syntax of '<tuple>'.  When 'ls|ll'\n" \
                                       "  is used, a list of Model points is generated. The <filter> argument will only\n" \
                                       "  list points that contain <filter>.  The '!' and '^' will lock and unlock the\n" \
                                       "  entire point respectively.  The '?' will set the entire point to the invalid\n"
                                       "  state.  The <num> is an optional invalid state value (range: 1 - 127)."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



///
namespace Rte { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class PUpdate: public Command
{
protected:
    /// List of supported points
    Cpl::Container::Map<Rte::TShell::Dac::Point>& m_points;


public:
    /// See Cpl::TShell::Dac::Command
    const char* getUsage() const throw()   { return RTETSHELLDACMD_USAGE_PUPDATE_; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const throw()    { return RTETSHELLDACMD_DETAIL_PUPDATE_; }
    
     
public:
    /// Constructor
    PUpdate( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList ) throw();

    /// Constructor.  Used to create a static instance of the command
    PUpdate( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw();


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
