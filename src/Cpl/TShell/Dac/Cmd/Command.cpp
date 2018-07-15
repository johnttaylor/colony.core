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

#include "Command.h"


///
using namespace Cpl::TShell::Dac::Cmd;


////////////////////////////
Command::Command( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb  ) throw()
:m_mapKey(verb)
    {
    commandList.insert( *this );
    }


Command::Command( Cpl::Container::Map<Cpl::TShell::Dac::Command>& commandList, const char* verb, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance  ) throw()
:Cpl::TShell::Dac::Command(ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
,m_mapKey(verb)
    {
    commandList.insert( *this );
    }


////////////////////////////
const char* Command::getVerb() const throw()                 
    { 
    return m_mapKey.getKeyValue(); 
    }


const Cpl::Container::Key& Command::getKey() const throw()   
    { 
    return m_mapKey; 
    }
