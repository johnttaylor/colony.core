#ifndef Cpl_Rte_RmwComposer_h_
#define Cpl_Rte_RmwComposer_h_
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

#include "Cpl/Rte/ModelPoint.h"

///
namespace Cpl {
///
namespace Rte {


/** This template class is a composer pattern/class that manages the callback
    function for a Model Point's read-modify-write operation.  

    A Composer is a structural pattern that may be used to employ composition
    when implementing an interface rather than using multiple inheritance. This
    allows a single concrete object to receive the RMW callbacks from many
    Model Points.

    Template Arguments:
        CONTEXT - The class that implements the Callback function
        DATA    - The type of the Model Point Data instance.
 */
template <class CONTEXT, class DATA>
class RmwComposer : public ModelPointRmwCallback<DATA>
{
public:
    /** Define a callback method function for the Modify Point callback (See
        Cpl::Rte::ModelPoint::RmwCallback for additional details)
     */
    typedef ModelPoint::RmwCallbackResult_T (CONTEXT::*ModifyFunc_T)(DATA& data, int8_t validState);


protected:
    /// Class the implement the callback
    CONTEXT&                    m_context;

    /// Method (in my Context) to call to perform the modify operation
    ModifyFunc_T                m_modifyCb;


public:
    /// Constructor
    RmwComposer( CONTEXT&       context,
                 ModifyFunc_T   modifyCallback );


public:
    /// See Cpl::Rte::ModelPointRmwCallback<DATA>
    ModelPoint::RmwCallbackResult_T callback( DATA& data, int8_t validState ) throw();

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <class CONTEXT, class DATA>
Cpl::Rte::RmwComposer<CONTEXT, DATA>::RmwComposer( CONTEXT&       context,
                                                   ModifyFunc_T   modifyCallback )
    :m_context( context )
    , m_modifyCb( modifyCallback )
{
}

/////////////////
template <class CONTEXT, class DATA>
Cpl::Rte::ModelPoint::RmwCallbackResult_T Cpl::Rte::RmwComposer<CONTEXT, DATA>::callback( DATA& data, int8_t validState) throw()
{
    // Notify context
    return (m_context.*m_modifyCb)( data, validState );
}


};      // end namespaces
};
#endif  // end header latch