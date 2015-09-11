#ifndef Rte_TShell_Dac_Cmd_RteWrite_h
#define Rte_TShell_Dac_Cmd_RteWrite_h
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
#define RTETSHELLDACMD_USAGE_RTEWRITE_  "rte-write ls|ll [<filter>]\n" \
                                        "rte-write <point> {(<tuple0>)[,(<tuple1>)]*}\n" \
                                        "rte-write <point> <idx> ([!|^] <[?[<num>]]|<elm0>>[,[!|^] <[?[<num>]]|<elm0>>]*)\n" \

/// Detailed Help text
#ifndef RTETSHELLDACMD_DETAIL_RTEWRITE_
#define RTETSHELLDACMD_DETAIL_RTEWRITE_ "  Writes an entire RTE Model point or tuple.  <point> is the symbolic name of a\n" \
                                        "  RTE Model Point.  <idx> is the zero based tuple index when selecting a single\n" \
                                        "  tuple to update. When 'ls|ll' is used, a list of Model points is generated.\n" \
                                        "  The <filter> argument will only list points that contain <filter>.  The '!'\n" \
                                        "  and '^' will lock and unlock the element respectively.  The '?' will set the\n" \
                                        "  element to the invalid state.  The <num> is an optional invalid state value\n" \
                                        "  (range: 1 - 127)."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



///
namespace Rte { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class RteWrite: public Command
{
protected:
    /// List of supported points
    Cpl::Container::Map<Rte::TShell::Dac::Point>& m_points;


public:
    /// See Cpl::TShell::Dac::Command                                                               `
    const char* getUsage() const throw()   { return RTETSHELLDACMD_USAGE_RTEWRITE_; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const throw()    { return RTETSHELLDACMD_DETAIL_RTEWRITE_; }
    
     
public:
    /// Constructor
    RteWrite( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList ) throw();

    /// Constructor.  Used to create a static instance of the command
    RteWrite( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw();


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
