#ifndef Cpl_TShell_Dac_ActiveVariables_x_h_
#define Cpl_TShell_Dac_ActiveVariables_x_h_
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

#include "Cpl/TShell/Dac/ActiveVariablesApi_.h"
#include "Cpl/TShell/Dac/Variable_.h"
#include "Cpl/Container/Map.h"
#include "Cpl/Memory/SPool.h"

///
namespace Cpl { namespace TShell { namespace Dac {

/** This Private Namespace concrete Template class implements the Active DAC 
    Variables interface.  

    Template ARGS:
        NUMVARS - The maximum number of allocted/supported variables
 */
template<int NUMVARS>
class ActiveVariables_: public ActiveVariablesApi_
{
protected:    
    /// Storage for variables
    Cpl::Memory::SPool<VariableApi_,NUMVARS> m_variableMem;

    /// Sorted list of active/in-use variables
    Cpl::Container::Map<VariableApi_>        m_inUse;


public:
    /// Constructor
    ActiveVariables_();

    /// Special Construct to be used when creating an instance statically!
    ActiveVariables_( const char* ignoreThisParameter_usedToCreateAUniqueConstructor );



public:
    /// See Cpl::TShell::Dac::ActiveVariablesApi_
    VariableApi_* get( Cpl::Text::String& variableName ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi_
    VariableApi_* find( Cpl::Text::String& variableName ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi_
    void remove( VariableApi_& varNoLongerInUse ) throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi_
    VariableApi_* first() const throw();

    /// See Cpl::TShell::Dac::ActiveVariablesApi_
    VariableApi_* next( VariableApi_& current ) const throw();
};


/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////

template<int NUMVARS>
ActiveVariables_<NUMVARS>::ActiveVariables_()
    {
    }

template<int NUMVARS>
ActiveVariables_<NUMVARS>::ActiveVariables_(const char* ignoreThisParameter_usedToCreateAUniqueConstructor)
 :m_inUse(ignoreThisParameter_usedToCreateAUniqueConstructor) 
    {
    }



template<int NUMVARS>
VariableApi_* ActiveVariables_<NUMVARS>::get( Cpl::Text::String& variableName ) const throw()
    {    
    // Handle the case: Variable already exists
    VariableApi_* varPtr = m_inUse.find( variableName );
    if ( varPtr )
        {
        return varPtr;
        }

    // Case: Craete new variable
    void* memPtr = m_varMemory.allocate( sizeof(Variable_) );
    if ( memPtr )
        {
        VariableApi_* varPtr = new(memPtr) Variable( variableName );
        m_inUse.insert( *varPtr );
        return varPtr;
        }

    // If I get here, I failed to create a new variable
    return 0;
    }



template<int NUMVARS>
VariableApi_* ActiveVariables_<NUMVARS>::find( Cpl::Text::String& variableName ) const throw()
    {
    return m_inUse.find( variableName );
    }

template<int NUMVARS>
void ActiveVariables_<NUMVARS>::remove( VariableApi_& varNoLongerInUse ) const throw()
    {
    m_inUse.removeItem( varNoLongerInUse );
    m_variableMem.release( &varNoLongerInUse );
    }



template<int NUMVARS>
VariableApi_* ActiveVariables_<NUMVARS>::first() const throw()
    {
    return m_inUse.first();
    }

template<int NUMVARS>
VariableApi_* ActiveVariables_<NUMVARS>::next( VariableApi_& current ) const throw()
    {
    return m_inUse.next( current );
    }


};      // end namespaces
};
};
#endif  // end header latch
