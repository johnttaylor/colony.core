#ifndef Rte_TShell_Dac_Cmd_Command_h_
#define Rte_TShell_Dac_Cmd_Command_h_
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
#include "Cpl/Container/Map.h"
#include "Rte/TShell/Dac/Point.h"


///
namespace Rte { namespace TShell { namespace Dac { namespace Cmd {

/** This partially concrete class implements infrastructure and/or common 
    functionality for a RTE DAC Shell command.
 */
class Command: public Cpl::TShell::Dac::Cmd::Command
{
protected:
    /// List of supported points
    Cpl::Container::Map<Rte::TShell::Dac::Point>&   m_points;


protected:
    /// Constructor
    Command( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList ) throw();

    /// Constructor.  Used to create a static instance of the command
    Command( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb, Cpl::Container::Map<Rte::TShell::Dac::Point>& modelPointList, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance ) throw();


protected:
    /// Helper: List all points
    virtual Cpl::TShell::Dac::Command::Result_T listPoints( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens ) throw();

    /// Helper: Formats Point Info
    virtual void outputPointInfo( Cpl::TShell::Dac::Context_& context, bool& io, Cpl::Text::String& outbuffer, Rte::TShell::Dac::Point& dacPoint, bool includeDetails=false );

    /// Helper: Formats the symbolic name of the point (based on if the Point is a Container)
    virtual void formatPointName( Cpl::Text::String& outbuffer, Rte::TShell::Dac::Point& dacPoint );

};

};      // end namespaces
};
};
};
#endif  // end header latch
