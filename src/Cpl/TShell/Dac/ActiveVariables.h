#ifndef Cpl_TShell_Dac_ActiveVariables_h_
#define Cpl_TShell_Dac_ActiveVariables_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
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

    /// Count of active user variable
    unsigned                              m_activeCount;

    /// System Variable prefix
    const char                            m_sysVarPrefix;
    
     
public:
    /// Constructor
    ActiveVariables( char systemVarPrefix = '_' );

    /// Special Construct to be used when creating an instance statically!
    ActiveVariables( const char* ignoreThisParameter_usedToCreateAUniqueConstructor, char systemVarPrefix = '_' );



public:
    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* get( const Cpl::Container::Key& variableName ) noexcept;

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* find( const Cpl::Container::Key& variableName ) noexcept;

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    void remove( VariableApi& varNoLongerInUse ) noexcept;

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    unsigned getUserCount() const noexcept;

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    unsigned getMaxUserCount() const noexcept;


    /// See Cpl::TShell::Dac::ActiveVariablesApi
    void addSystem( VariableApi& systemVariable ) noexcept;


    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* first() const noexcept;

    /// See Cpl::TShell::Dac::ActiveVariablesApi
    VariableApi* next( VariableApi& current ) const noexcept;
};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////

template<int NUMVARS>
ActiveVariables<NUMVARS>::ActiveVariables( char systemVarPrefix )
:m_activeCount(0)
,m_sysVarPrefix(systemVarPrefix)
    {
    }

template<int NUMVARS>
ActiveVariables<NUMVARS>::ActiveVariables( const char* ignoreThisParameter_usedToCreateAUniqueConstructor, char systemVarPrefix )
:m_inUse(ignoreThisParameter_usedToCreateAUniqueConstructor) 
,m_activeCount(0)
,m_sysVarPrefix(systemVarPrefix)
    {
    }



template<int NUMVARS>
VariableApi* ActiveVariables<NUMVARS>::get( const Cpl::Container::Key& variableName ) noexcept
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
        m_activeCount++;
        return varPtr;
        }

    // If I get here, I failed to create a new variable
    return 0;
    }



template<int NUMVARS>
VariableApi* ActiveVariables<NUMVARS>::find( const Cpl::Container::Key& variableName ) noexcept
    {
    return m_inUse.find( variableName );
    }

template<int NUMVARS>
void ActiveVariables<NUMVARS>::remove( VariableApi& varNoLongerInUse ) noexcept
    {
    if ( *(varNoLongerInUse.getName()) != m_sysVarPrefix )
        {
        m_activeCount--;
        m_inUse.removeItem( varNoLongerInUse );
        m_variableMem.release( &varNoLongerInUse );
        }
    }

template<int NUMVARS>
void ActiveVariables<NUMVARS>::addSystem( VariableApi& systemVariable )  noexcept
    {
    m_inUse.insert( systemVariable );
    }


template<int NUMVARS>
unsigned ActiveVariables<NUMVARS>::getUserCount() const noexcept
    {
    return m_activeCount;
    }


template<int NUMVARS>
unsigned ActiveVariables<NUMVARS>::getMaxUserCount() const noexcept
    {
    return NUMVARS;
    }


template<int NUMVARS>
VariableApi* ActiveVariables<NUMVARS>::first() const noexcept
    {
    return m_inUse.first();
    }

template<int NUMVARS>
VariableApi* ActiveVariables<NUMVARS>::next( VariableApi& current ) const noexcept
    {
    return m_inUse.next( current );
    }


};      // end namespaces
};
};
#endif  // end header latch
