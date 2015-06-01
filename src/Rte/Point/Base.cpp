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
:m_seqnum(0)
,m_updated(false)
    {
    }

/////////////////
bool Base::isUpdated( void ) const
    {
    return m_updated;
    }

void Base::clearUpdatedState( void )
    {
    m_updated = false;
    }

void Base::setUpdatedState( void )
    {
    m_updated = true;
    }


void Base::incrementSequenceNumber(void)
    {
    // Do not allow a sequence number of zero (zero represents 'unknown sequence number')
    if ( ++m_seqnum == 0 )
        {
        m_seqnum = 1;
        }
    }

uint32_t Base::getSequenceNumber(void) const
    {
    return m_seqnum;
    }

void Base::setSequenceNumber( uint32_t newSeqNum )
    {
    m_seqnum = newSeqNum;
    }

void Base::invalidateSequenceNumber(void)
    {
    m_seqnum = 0;
    }

bool Base::isDifferent( Api& other ) const
    {
    if ( m_seqnum == 0 || other.getSequenceNumber() == 0 || m_seqnum != other.getSequenceNumber() )
        {
        return true;
        }

    return false;
    }


////////////////////////
void Base::copyFrom( Api& other, Api* inUseFilterP )
    {
    // Trap mixed-match points
    if ( getNumTuples() != other.getNumTuples() )
        {
        Cpl::System::FatalError::logf( "Rte::Point::Base::copyFrom(point) - otherTuple's tuple count does not match (my count=%u, other count=%u)", getNumTuples(), other.getNumTuples() );
        }
    
    // Mark the Point state: as 'updated'
    setUpdatedState();
    incrementSequenceNumber();

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
    
    // Clear the 'destination point' (aka the point that might be updated) as not-updated
    other.clearUpdatedState();

    // Do the compare by SEQUENCE number
    bool pointDifferent = false;
    if ( !compareValues )
        {
        if ( isDifferent(other) )
            {
            pointDifferent = true;

            unsigned j;
            for(j=0; j<getNumTuples(); j++)
                {
                Tuple::Api::copy( other.getTuple(j), getTuple(j), allElements? 0: &(other.getTuple(j)) );
                }
            }
        }

    // Do the compare by VALUE
    else
        {
        unsigned j;
        for(j=0; j<getNumTuples(); j++)
            {
            pointDifferent |= Rte::Tuple::Api::compareAndCopy( getTuple(j), other.getTuple(j), allElements, true );
            }
        }

    // Update the 'Others' seqeuence number (and changed-state) to match this Point if/when there was a difference between the two
    if ( pointDifferent )
        {
        other.setSequenceNumber( getSequenceNumber() );
        other.setUpdatedState();
        }

    // Return true if at least one tuple was different (and updated)
    return pointDifferent;
    }


////////////////////////
void Base::setAllValidFlagState( bool newState )
    {
    unsigned i;
    for(i=0; i<getNumTuples(); i++)
        {
        getTuple(i).setAllValidFlagState( newState );
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


