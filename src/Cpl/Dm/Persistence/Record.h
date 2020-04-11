#ifndef Cpl_Dm_Persistence_Record_h_
#define Cpl_Dm_Persistence_Record_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Persistence/Record.h"
#include "Cpl/Persistence/Chunk.h"
#include "Cpl/Persistence/Payload.h"
#include "Cpl/Dm/ModelPoint.h"
#include "Cpl/Dm/SubscriberComposer.h"

///
namespace Cpl {
///
namespace Dm {
///
namespace Persistence {

/** This mostly concrete class implements the Cpl::Persistence::Record interface
    where a Record instance contains the data from N model points.  A final
    child class is needed to provide the specifics of 'resetting' the Record's
    data.
 */
class Record: public Cpl::Persistence::Record, public Cpl::Persistence::Payload
{
public:
    /** This data structure associates a Data Model subscriber instance with a
        Model Point instance.
     */
    typedef struct
    {
        Cpl::Dm::ModelPoint*                                        mpPtr;          //!< Pointer to a Model Point 
        Cpl::Dm::SubscriberComposer<Record,Cpl::Dm::ModelPoint>*    observerPtr;    //!< Place holder to for dynamically allocated Subscriber Instance.  Note: A ZERO value will not allocate a subscriber
    } Item_T;

public:
    /** Constructor. The 'itemList' is variable length array where the last item
        in the list MUST be a 'null' entry, e.g. {0,0}.  The application is 
        responsible for ALLOCATING all of the Model Point and Subscriber 
        instances in the itemList.
     */
    Record( Item_T itemList[], Cpl::Persistence::Chunk& chunkHandler ) noexcept;

    /// Destructor
    ~Record();


public:
    /// See Cpl::Persistence::Record
    void start( Cpl::Itc::PostApi& myMbox, Cpl::Dm::EventLoop& myEventLoop ) noexcept;

    /// See Cpl::Persistence::Record
    void stop() noexcept;

    /// See Cpl::Persistence::Payload
    size_t getData( void* dst, size_t maxDstLen ) noexcept;

    /// See Cpl::Persistence::Payload
    bool putData( const void* src, size_t srcLen ) noexcept;


protected:
    /** This method is responsible for updating all of the Model Points in
        record to their default values.  This method is called when there is
        valid data when reading the record's data from persistence storage.
     */
    virtual void resetData() noexcept = 0;


protected:
    /// Callback method for Model Point change notifications
    virtual void dataChanged( Cpl::Dm::ModelPoint& point ) noexcept;


protected:
    /// List of model points
    Item_T*                     m_items;

    /// Chunk handler for the Record
    Cpl::Persistence::Chunk&    m_chunkHandler;

    /// Remember my started state
    bool                        m_started;
};

/// Magic value to indicate that Subscriber instance should be allocated for Model Point item
#define CPL_DM_PERISTENCE_RECORD_USE_SUBSCRIBER     ((Cpl::Dm::SubscriberComposer<Record,Cpl::Dm::ModelPoint>*) 1 )

/// Magic value to indicate that Subscriber instance should NOT be allocated for Model Point item
#define CPL_DM_PERISTENCE_RECORD_NO_SUBSCRIBER      ((Cpl::Dm::SubscriberComposer<Record,Cpl::Dm::ModelPoint>*) 0 )



};      // end namespaces
};
#endif  // end header latch