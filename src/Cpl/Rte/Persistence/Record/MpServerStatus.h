#ifndef Cpl_Rte_Persistence_Record_MpServerStatus_h_
#define Cpl_Rte_Persistence_Record_MpServerStatus_h_
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


#include "Cpl/Rte/Persistence/Record/ServerStatus.h"
#include "Cpl/Rte/Mp/Enum.h"
#include "Cpl/Rte/Subscriber.h"

///
namespace Cpl {
namespace Rte {
namespace Persistence {
namespace Record {

/** Concrete Model Point for publishing the Record Server's current state
 */
class MpServerStatus : public Cpl::Rte::Mp::Enum<Cpl::Rte::Persistence::Record::ServerStatus>
{
public:
    /// Constructor. 
    MpServerStatus( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo );

    /// Constructor. 
    MpServerStatus( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, ServerStatus initialValue );

    // Type
    const char* getTypeAsText() const throw();

public:
    /// Type safe subscriber
    typedef Cpl::Rte::Subscriber<MpServerStatus> Observer;

    /// Type safe register observer
    void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw();

    /// Type safe un-register observer
    void detach( Observer& observer ) throw();
};


};      // end namespaces
};
};
};
#endif  // end header latch
