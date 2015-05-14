#ifndef Cpl_TShell_Dac_Cmd_Try_h
#define Cpl_TShell_Dac_Cmd_Try_h
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
#include "Cpl/Container/Stack.h"

                                                 
/** This symbol defines the number of levels an IF/ELSE construct can be nested
 */
#ifndef OPTION_CPL_TSHELL_DAC_CMD_TRY_IFELSE_NEST_LEVELS
#define OPTION_CPL_TSHELL_DAC_CMD_TRY_IFELSE_NEST_LEVELS    4
#endif

/* RULER
                                      "         1         2         3         4         5         6         7         8"
                                      "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
*/
#define CPLTSHELLDACMD_USAGE_TRY_     "try IF <op1> <test> <op2> [AND|OR <op3> <test> <op4>]...\n" \
                                      "try ELSE\n" \
                                      "try ELIF <op1> <test> <op2> [AND|OR <op3> <test> <op4>]...\n" \
                                      "try ENDIF"
#ifndef CPLTSHELLDACMD_DETAIL_TRY_
#define CPLTSHELLDACMD_DETAIL_TRY_    "  Provides an if/else construct.  Valid <test> operations are: '=', '!=', '<',\n" \
                                      "  '>', '>=', and '<='.  The operands (e.g. <op1>) can be a shell variable or a\n" \
                                      "  literal value (a literal is prefixed with a '#').  The AND|OR clauses are\n" \
                                      "  evaluated stricly left to right. The AND|OR clauses will 'short-circuit' when\n" \
                                      "  the comparison results are false|true respectively." 


#endif // ifndef allows detailed help to be compacted down to a single character if FLASH/code space is an issue

///
namespace Cpl { namespace TShell { namespace Dac { namespace Cmd {



/** This class implements a DAC Shell command
 */
class Try: public Command_
{
protected:         
    /// States for my top level FSM
    enum State_T { eIDLE,           //!< Outside of an IF/ELSE construct
                   eCLAUSE_FALSE,   //!< Processing the false clause if the IF/ELSE construct
                   eCLAUSE_TRUE,    //!< Processing the true clause if the IF/ELSE construct
                   eCOMPARE_ERROR   //!< Error occuring during the evaulation of the test/compare operation
                 }; 


protected:
    /// My FSM state
    State_T                         m_state;

    /// Keep track of my nest IF/ELSE (when evaluating the false clauses)
    unsigned                        m_level;

    /// State Stack for nested IF/ELSE
    Cpl::Container::Stack<State_T>  m_stack;

    /// Memory for my state stack
    State_T                         m_memStack[OPTION_CPL_TSHELL_DAC_CMD_TRY_IFELSE_NEST_LEVELS];

public:
    /// See Cpl::TShell::Dac::Command_
    const char* getUsage() const throw()    { return CPLTSHELLDACMD_USAGE_TRY_; }

    /// See Cpl::TShell::Dac::Command_
    const char* getHelp() const throw()    { return CPLTSHELLDACMD_DETAIL_TRY_; }
    
     
public:
    /// Constructor
    Try( Cpl::Container::Map<Cpl::TShell::Dac::Command_>& commandList ) throw();


public:
    /// See Cpl::TShell::Dac::Command_
    Cpl::TShell::Dac::Command_::Result_T execute( Cpl::TShell::Dac::Context_& context, Cpl::Text::Tokenizer::TextBlock& tokens, const char* rawInputString, Cpl::Io::Output& outfd ) throw();
            



protected:
    /// Helper method
    virtual State_T popState() throw();

    /// Helpter method
    virtual State_T convert2State( Cpl::TShell::Dac::Cmd::Command_::CondResult_T result ) const throw();



};

};      // end namespaces
};
};
};
#endif  // end header latch
