#ifndef Cpl_Rte_Persistence_Record_MpServerStatus_h_
#define Cpl_Rte_Persistence_Record_MpServerStatus_h_
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


#include "Cpl/Dm/Persistence/Record/ServerStatus.h"
#include "Cpl/Dm/Mp/Enum.h"
#include "Cpl/Dm/Subscriber.h"

///
namespace Cpl {
namespace Dm {
namespace Persistence {
namespace Record {

/** Concrete Model Point for publishing the Record Server's current state
 */
class MpServerStatus : public Cpl::Dm::Mp::Enum<Cpl::Dm::Persistence::Record::ServerStatus>
{
public:
    /// Constructor. 
    MpServerStatus( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo )
		:Cpl::Dm::Mp::Enum<ServerStatus>( myModelBase, staticInfo )
	{
	}

    /// Constructor. 
    MpServerStatus( Cpl::Dm::ModelDatabase& myModelBase, StaticInfo& staticInfo, ServerStatus initialValue )
		: Cpl::Dm::Mp::Enum<ServerStatus>( myModelBase, staticInfo, initialValue )
	{
	}

    // Type
    const char* getTypeAsText() const noexcept
	{
		return "Cpl::Dm::Persistence::Record::ServerStatus";
	}

public:
    /// Type safe subscriber
    typedef Cpl::Dm::Subscriber<MpServerStatus> Observer;

    /// Type safe register observer
    void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) noexcept
	{
		ModelPointCommon_::attach( observer, initialSeqNumber );
	}

    /// Type safe un-register observer
    void detach( Observer& observer ) noexcept
	{
		ModelPointCommon_::detach( observer );
	}
};


};      // end namespaces
};
};
};
#endif  // end header latch
