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

#include "Base.h"
#include "Cpl/System/FatalError.h"

///
using namespace Rte::Point;


////////////////////////
Base::Base( void )
:m_seqnum_(0)
,m_changed_(false)
    {
    }

/////////////////
void Base::incrementSequenceNumber(void)
    {
    // Do not allow a sequence number of zero (zero represents 'unknown sequence number')
    if ( ++m_seqnum_ == 0 )
        {
        m_seqnum_ = 1;
        }
    }

uint32_t Base::getSequenceNumber(void) const
    {
    return m_seqnum_;
    }

void Base::setSequenceNumber( uint32_t newSeqNum )
    {
    m_seqnum_ = newSeqNum;
    }

void Base::resetSequenceNumber(void)
    {
    m_seqnum_ = 0;
    }


void Base::clearMembershipChanged(void)
    {
    m_changed_ = false;
    }

void Base::setMembershipChanged(void)
    {
    m_changed_ = true;
    }

bool Base::isMembershipChanged(void) const
    {
    return m_changed_;
    }


bool Base::isContainer(void) const
    {
    return false;
    }

////////////////////////
void Base::invalidateAllTupleSequenceNumbers(void)
    {
    unsigned j;
    for(j=0; j<getNumTuples(); j++)
        {
        getTuple(j).invalidateSequenceNumber();
        }
    }


void Base::copyAllSequenceNumbersFrom( Api& other )
    {
    // Copy the Point's sequence number
    setSequenceNumber( other.getSequenceNumber() );

    // Copy the Tuples' sequence numbers
    unsigned j;
    for(j=0; j<getNumTuples(); j++)
        {
        getTuple(j).setSequenceNumber( other.getTuple(j).getSequenceNumber() );
        }
    }


////////////////////////
void Base::copyFrom( Api& other, Api* inUseFilterP )
    {
    // Trap mixed-match points
    if ( getNumTuples() != other.getNumTuples() )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Base::copyFrom(point) - otherTuple's tuple count does not match (my count=%u, other count=%u)", getNumTuples(), other.getNumTuples() );
        }
    
    // Detect membership updates (from the Controller)
    if ( other.getSequenceNumber() > 0 )
        {
        incrementSequenceNumber();
        }

    // Copy ALL tuples
    unsigned j;
    for(j=0; j<getNumTuples(); j++)
        {
        Rte::Tuple::Api::copy( getTuple(j), other.getTuple(j), !inUseFilterP? 0: &(inUseFilterP->getTuple(j)) );
        }
    }


bool Base::compareAndCopy( Api& other, bool allElements, bool compareValues )
    {
    // Trap mixed-matched points
    if ( getNumTuples() != other.getNumTuples() )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Base::compareAndCopy(point) - otherTuple's tuple count does not match (my count=%u, other count=%u)", getNumTuples(), other.getNumTuples() );
        }
    
    // Do the compare 
    bool     pointDifferent = false;
    unsigned j;
    for(j=0; j<getNumTuples(); j++)
        {
        pointDifferent |= Rte::Tuple::Api::compareAndCopy( getTuple(j), other.getTuple(j), allElements, compareValues );
        }

    // Update the other's (aka Viewer's) membership changed flag 
    other.clearMembershipChanged();
    if ( other.getSequenceNumber() != getSequenceNumber() )
        {
        other.setMembershipChanged();
        other.setSequenceNumber( getSequenceNumber() );

        // Ensure that the Point is marked as different (handles the case where the Point was 'touched' - but no tuple was actually changed)
        pointDifferent = true;  
        }

    // Return true if at least one tuple was different (and updated)
    return pointDifferent;
    }


////////////////////////
void Base::setAllValidState( int8_t newState )
    {
    unsigned i;
    for(i=0; i<getNumTuples(); i++)
        {
        getTuple(i).setAllValidState( newState );
        }
    }

void Base::setAllInUseState( bool newState )
    {
    unsigned i;
    for(i=0; i<getNumTuples(); i++)
        {
        getTuple(i).setAllInUseState( newState );
        }
    }

size_t Base::sumAllExternalSize( void )
    {
    size_t   sum = 0;
    unsigned i;
    for(i=0; i<getNumTuples(); i++)
        {
        sum += getTuple(i).sumAllExternalSize(); 
        }

    return sum;
    }


void Base::setAsModelPoint_(void)
    {
    unsigned j;
    for(j=0; j<getNumTuples(); j++)
        {
        getTuple(j).setAsModelTuple_();
        }
    }
    

