#ifndef Cpl_Rte_Subscriber_h_
#define Cpl_Rte_Subscriber_h_
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

#include "Cpl/Rte/SubscriberApi.h"

///
namespace Cpl {
///
namespace Rte {


/** This template class is a composer pattern/class that manages the Change
    notification callback function for a Model Point's Subscriber.  It also
    provides a concrete Subscriber

    A Composer is a structural pattern that may be used to employ composition
    when implementing an interface rather than using multiple inheritance. This
    allows a single concrete object to receive the Change Notification callbacks
    from many Model Points.

    Template Arguments:
        CONTEXT - The class that implements the Change Notification Callback function
        MP      - The concrete Model Point Type
 */
template <class CONTEXT, class MP>
class Subscriber : public SubscriberApi
{
public:
    /** Define Change Notification callback method function
     */
    typedef void (CONTEXT::*ModifyFunc_T)( MP& modelPoint ) throw();


protected:
    /// Reference to my containing instance
    CONTEXT&                    m_context;

    /// Method (in my Context) to call to perform the modify operation
    ModifyFunc_T                m_modifyCb;


public:
    /// Constructor
    Subscriber( CONTEXT&       context,
                ModifyFunc_T   modifyCallback );

protected:
    /// See Cpl::Rte::SubscriberApi
    void genericModelPointChanged( ModelPoint& modelPointThatChanged ) throw();
};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <class CONTEXT, class MP>
Cpl::Rte::Subscriber<CONTEXT, MP>::Subscriber( CONTEXT&       context,
                                               ModifyFunc_T   modifyCallback )
    : m_context( context )
    , m_modifyCb( modifyCallback )
{
}

/////////////////
template <class CONTEXT, class MP>
void Cpl::Rte::Subscriber<CONTEXT, MP>::genericModelPointChanged( ModelPoint& modelPointThatChanged ) throw()
{
    // Notify context
    (m_context.*m_modifyCb)( *((MP*)&modelPointThatChanged) );
}


};      // end namespaces
};
};
#endif  // end header latch
