#ifndef Cpl_TShell_Dac_Cmd_Loop_h
#define Cpl_TShell_Dac_Cmd_Loop_h
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
#include "Cpl/TShell/Dac/Cmd/Command_.h"
#include "Cpl/TShell/Dac/Processor.h"

                                                 
/* RULER
                                      "         1         2         3         4         5         6         7         8"
                                      "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define CPLTSHELLDACMD_USAGE_LOOP_    "loop BEGIN\n" \
                                      "loop UNTIL [<op1> <test> <op2> [AND|OR <op3> <test> <op4>]]...\n" \
                                      "loop BREAK [<op1> <test> <op2> [AND|OR <op3> <test> <op4>]]..."
#ifndef CPLTSHELLDACMD_DETAIL_LOOP_
#define CPLTSHELLDACMD_DETAIL_LOOP_   "  Provides a looping construct. When 'UNTIL' is encounter, the previous\n" \
                                      "  command(s) are replayed starting with 'BEGIN'.  The 'BREAK' directive will\n" \
                                      "  exit the loop (i.e. begin executing the first command after 'UNTIL').  An\n" \
                                      "  optional expression can be used with 'UNTIL' and 'BREAK' directives.  For the\n" \
                                      "  'UNTIL' directive, true means exit the loop.  For the 'BREAK' directive, true\n" \
                                      "  means exit the loop."

#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue

///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class Loop: public Command_
{
protected:         
    /// States for my top level FSM
    enum State_T { eIDLE,           //!< Outside of any loop
                   eCAPTURE_LOOP,   //!< First time through the loop (i.e. capturing the commands in the loop)
                   eLOOPING,        //!< Executing the loop (i.e. it is at least the second time I have been in the loop)
                   eBREAKING,       //!< Exiting the loop (on the first pass through the loop)
                   eCOMPARE_ERROR   //!< Error occuring during the evaulation of the test/compare operation
                 }; 


protected:
    /// My FSM state
    State_T  m_state;

    /// Track the number of nested levels
    unsigned m_level;


public:
    /// See Cpl::TShell::Dac::Command_
    const char* getUsage() const throw()    { return CPLTSHELLDACMD_USAGE_LOOP_; }

    /// See Cpl::TShell::Dac::Command_
    const char* getHelp() const throw()    { return CPLTSHELLDACMD_DETAIL_LOOP_; }
    
     
public:
    /// Constructor
    Loop( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw();


public:
    /// See Cpl::TShell::Dac::Command_
    Cpl::TShell::Dac::Command_::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();
          
            
};

};      // end namespaces
};
};
};
#endif  // end header latch
