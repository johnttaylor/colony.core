#ifndef Cpl_TShell_Dac_ActiveVariables_h_
#define Cpl_TShell_Dac_ActiveVariables_h_
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

#include "Cpl/TShell/Dac/ActiveVariablesApi.h"
#include "Cpl/TShell/Dac/Variable_.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Memory/SPool.h"

///
namespace Cpl { namespace TShell { namespace Dac {

/** This concrete Template class implements the Active DAC Variables interface.

    Template ARGS:
        NUMVARS - The maximum number of allocted/supported variables
 */
template<int NUMVARS>
class ActiveVariables: public ActiveVariablesApi
{
protected:    
    /// Storage for variables
    Cpl::Memory::SPool<Variable_,NUMVARS> m_variableMem;

    /// Sorted list of active/in-use variables
    Cpl::Container::Map<VariableApi>      m_inUse;


public:
    /// Constructor
    ActiveVariables();

    /// Special Construct to be used when creating an instance statically!
    ActiveVariables( const char* ignoreThisParameter_usedToCreateAUniqueConstructor );



public:
    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* get( Cpl::Container::Key& variableName ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* find( Cpl::Container::Key& variableName ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    void remove( VariableApi& varNoLongerInUse ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    unsigned getMaxCount() const throw();


    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* first() const throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* next( VariableApi& current ) const throw();
};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////

template<int NUMVARS>
ActiveVariables<NUMVARS>::ActiveVariables()
    {
    }

template<int NUMVARS>
ActiveVariables<NUMVARS>::ActiveVariables(const char* ignoreThisParameter_usedToCreateAUniqueConstructor)
 :m_inUse(ignoreThisParameter_usedToCreateAUniqueConstructor) 
    {
    }



template<int NUMVARS>
VariableApi* ActiveVariables<NUMVARS>::get( Cpl::Container::Key& variableName ) throw()
    {    
    // Handle the case: Variable already exists
    VariableApi* varPtr = m_inUse.find( variableName );
    if ( varPtr )
        {
        return varPtr;
        }

    // Case: Craete new variable
    void* memPtr = m_variableMem.allocate( sizeof(Variable_) );
    if ( memPtr )
        {
        VariableApi* varPtr = new(memPtr) Variable_( (const char*) variableName.getRawKey() );
        m_inUse.insert( *varPtr );
        return varPtr;
        }

    // If I get here, I failed to create a new variable
    return 0;
    }



template<int NUMVARS>
VariableApi* ActiveVariables<NUMVARS>::find( Cpl::Container::Key& variableName ) throw()
    {
    return m_inUse.find( variableName );
    }

template<int NUMVARS>
void ActiveVariables<NUMVARS>::remove( VariableApi& varNoLongerInUse ) throw()
    {
    m_inUse.removeItem( varNoLongerInUse );
    m_variableMem.release( &varNoLongerInUse );
    }

template<int NUMVARS>
unsigned ActiveVariables<NUMVARS>::getMaxCount() const throw()
    {
    return NUMVARS;
    }


template<int NUMVARS>
VariableApi* ActiveVariables<NUMVARS>::first() const throw()
    {
    return m_inUse.first();
    }

template<int NUMVARS>
VariableApi* ActiveVariables<NUMVARS>::next( VariableApi& current ) const throw()
    {
    return m_inUse.next( current );
    }


};      // end namespaces
};
};
#endif  // end header latch
