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

#include "NullVariables.h"

///
using namespace Cpl::TShell::Dac;



//////////////////////////////////////////
NullVariables::NullVariables()
    {
    }

NullVariables::NullVariables( const char* ignoreThisParameter_usedToCreateAUniqueConstructor )
:m_inUse(ignoreThisParameter_usedToCreateAUniqueConstructor)
    {
    }


//////////////////////////////////////////
VariableApi* NullVariables::get( Cpl::Container::Key& variableName ) throw()
    {
    return m_inUse.find( variableName );
    }


VariableApi* NullVariables::find( Cpl::Container::Key& variableName ) throw()
    {
    return m_inUse.find( variableName );
    }


void NullVariables::remove( VariableApi& varNoLongerInUse ) throw()
    {
    }


unsigned NullVariables::getUserCount() const throw()
    {
    return 0;
    }


unsigned NullVariables::getMaxUserCount() const throw()
    {
    return 0;
    }


VariableApi* NullVariables::first() const throw()
    {
    return m_inUse.first();
    }


VariableApi* NullVariables::next( VariableApi& current ) const throw()
    {
    return m_inUse.next( current );
    }
