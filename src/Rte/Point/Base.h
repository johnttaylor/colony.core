#ifndef Rte_Point_Base_h_
#define Rte_Point_Base_h_
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


#include "Rte/Point/Api.h"


///
namespace Rte { namespace Point {


/** This partially concrete class provide common infrastructure for
    a Point. 
 */
class Base: public Rte::Point::Api
{
protected:
    /// Internal sequence number to track changes to the Point
    uint32_t m_seqnum;

    /// Was this Point updated/modified.
    bool     m_updated;


protected:
    /// Constructor
    Base( void );


public: 
    /// See Rte::Point::Api
    void copyFrom( Api& other, Api* inUseFilterP=0 );

    /// See Rte::Point::Api
    bool compareAndCopy( Api& other, bool allElements=false, bool compareValues=true );

    /// See Rte::Point::Api
    void setAllValidFlagState( bool newState );

    /// See Rte::Point::Api
    void setAllInUseState( bool newState );

    /// See Rte::Point::Api
    size_t sumAllExternalSize( void );

    /// See Rte::Point::Api
    bool isUpdated(void) const;

    /// See Rte::Point::Api
    void clearUpdatedState(void);

    /// See Rte::Point::Api
    void setUpdatedState(void);

    /// See Rte::Point::Api
    void incrementSequenceNumber(void);

    /// See Rte::Point::Api
    uint32_t getSequenceNumber(void) const;

    /// See Rte::Point::Api
    void setSequenceNumber( uint32_t newSeqNum );
    
    /// See Rte::Point::Api
    void invalidateSequenceNumber(void);

    /// See Rte::Point::Api
    bool isDifferent( Api& other ) const;
};

};      // end namespaces
};
#endif  // end header latch
