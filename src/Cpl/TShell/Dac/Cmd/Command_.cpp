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

#include "Command_.h"


///
using namespace Cpl::TShell::Dac;


////////////////////////////
Command_::Command_( Cpl::Container::Map<Command_>& commandList, const char* verb, const char* usage, const char* help ) throw()
:m_verb(verb)
,m_usage(usage)
,m_help(help)
    {
    commandList.insert( *this );
    }

////////////////////////////
const char* Command_::getVerb() const throw()
    {
    return m_verb;
    }

const char* Command_::getUsage() const throw()
    {
    return m_usage;
    }

const char* Command_::getHelp() const throw()
    {
    return m_help;
    }

