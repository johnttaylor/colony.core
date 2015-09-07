#ifndef Rte_TShell_Dac_Cmd_TUpdate_h
#define Rte_TShell_Dac_Cmd_TUpdate_h
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
#include "Cpl/TShell/Dac/Cmd/Command.h"


/** Usage
                                       "         1         2         3         4         5         6         7         8"
                                       "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define RTETSHELLDACMD_USAGE_TUPDATE_  "tupdate <point>\n" \ 
                                       "tupdate <point> <idx> <tuple_args>"

/// Detailed Help text
#ifndef RTETSHELLDACMD_DETAIL_TUPDATE_
#define RTETSHELLDACMD_DETAIL_TUPDATE_ "  Updates a single Tuple within a RTE Model point.  <point> is the symbolic \n" \
                                       "  name of a RTE Model Point and <idx> is the zero base index of the Tuple to\n" \
                                       "  update. When no arguments after <point> are provided the number of Tuples in\n" \
                                       "  point is provided.  The format of <tuple_args> is:\n" \
                                       "    <elem0>,[<elem1>]*  If the <elemN> is omitted then the corresponding\n "\
                                       "                        Element is NOT updated."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue



///
namespace Rte { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class TUpdate: public Command
{
protected:
    /// List of supported points
    Cpl::Container::Map<Rte::TShell::Dac::Point>& m_points;


public:
    /// See Cpl::TShell::Dac::Command
    const char* getUsage() const throw()   { return RTETSHELLDACMD_USAGE_TUPDATE_; }

    /// See Cpl::TShell::Dac::Command
    const char* getHelp() const throw()    { return RTETSHELLDACMD_DETAIL_TUPDATE_; }
    
     
public:
    /// Constructor
    TUpdate( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList ) throw();


public:
    /// See Cpl::TShell::Dac::Command
    Cpl::TShell::Dac::Command::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();

};

};      // end namespaces
};
};
};
#endif  // end header latch
