#ifndef Cpl_Rte_SubscriberComposer_h_
#define Cpl_Rte_SubscriberComposer_h_
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

#include "Cpl/Rte/Subscriber.h"

///
namespace Cpl {
///
namespace Rte {


/** This template class is a composer pattern/class that manages the Change
    notification callback function for a Model Point's Subscriber

    A Composer is a structural pattern that may be used to employ composition
    when implementing an interface rather than using multiple inheritance. This
    allows a single concrete object to receive the Change Notification callbacks 
    from many Model Points.

    Template Arguments:
        CONTEXT - The class that implements the Change Notification Callback function
        MP      - The concrete Model Point Type
        POINT   - The Model Point's concrete Point Type.
 */
template <class CONTEXT, class MP, class POINT>
class SubscriberComposer : public MP::RmwCallback
{
public:
    /** Define a callback method function for the Modify Point callback (See
        Cpl::Rte::ModelPoint::RmwCallback for additional details)
     */
    typedef void (CONTEXT::*ModifyFunc_T)(POINT& data, bool isValid) throw();


protected:
    /// Reference to the Model point being modified
    MP&                         m_modelPoint;

    /// Reference to my containing instance
    CONTEXT&                    m_context;

    /// Method (in my Context) to call to perform the modify operation
    ModifyFunc_T                m_modifyCb;


public:
    /// Constructor
    SubscriberComposer( MP&            modelPoint,
                 CONTEXT&       context,
                 ModifyFunc_T   modifyCallback );

    );


protected:
    /// See Cpl::Rte::ModelPoint::RmwCallback
    Result_T rmwCallback( POINT& data, bool isValid ) throw();

};

/////////////////////////////////////////////////////////////////////////////
//                  INLINE IMPLEMENTAION
/////////////////////////////////////////////////////////////////////////////
template <class CONTEXT, class MP, class POINT>
Cpl::Rte::SubscriberComposer<CONTEXT, MP, POINT>::SubscriberComposer( MP&            modelPoint,
                                                        CONTEXT&       context,
                                                        ModifyFunc_T   modifyCallback )
    :m_modelPoint( modelPoint )
    , m_context( context )
    , m_modifyCb( modifyCallback )
{
}

/////////////////
template <class CONTEXT, class MP, class POINT>
Cpl::Rte::ModelPoint::RmwCallback::Result_T Cpl::Rte::SubscriberComposer<CONTEXT, MP, POINT>::rmwCallback( POINT& data, bool isValid ) throw()
{
    // Notify context
    return (m_context.*m_modifyCb)( data, isValid );
}


};      // end namespaces
};
};
#endif  // end header latch
