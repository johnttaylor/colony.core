#ifndef Rte_Tuple_Base_h_
#define Rte_Tuple_Base_h_
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


#include "Rte/Tuple/Api.h"


///
namespace Rte { namespace Tuple {


/** This class provides common infrastructure for a tuple.
 */
class Base: public Api
{
protected:
    /// Internal sequence number to track changes to the Tuple
    uint32_t m_seqnum;

    /// Was this Tuple updated/modified.
    bool     m_updated;


protected:
    /// Constructor
    Base( void );


public: 
    /// See Rte::Tuple::Api
    bool isUpdated(void) const;

    /// See Rte::Tuple::Api
    void clearUpdatedState(void);

    /// See Rte::Tuple::Api
    void setUpdatedState(void);

    /// See Rte::Tuple::Api
    void incrementSequenceNumber(void);

    /// See Rte::Tuple::Api
    uint32_t getSequenceNumber(void) const;

    /// See Rte::Tuple::Api
    void setSequenceNumber( uint32_t newSeqNum );
    
    /// See Rte::Tuple::Api
    void invalidateSequenceNumber(void);

    /// See Rte::Tuple::Api
    bool isDifferent( Api& other ) const;

    /// See Rte::Tuple::Api
    void setAllValidFlagState( bool newState );

    /// See Rte::Tuple::Api
    void setAllInUseState( bool newState );

    /// See Rte::Tuple::Api
    size_t sumAllExternalSize(void) const;

};


};      // end namespaces
};
#endif  // end header latch
