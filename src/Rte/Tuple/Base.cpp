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


///
using namespace Rte::Tuple;


///////////////////////
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


/////////////////
void Base::setAllValidFlagState( bool newState ) 
    {
    unsigned i;
    for(i=0; i<getNumElements(); i++)
        {
        getElement(i).setValidFlagState(newState);
        }
    }

void Base::setAllInUseState( bool newState )
    {
    unsigned i;
    for(i=0; i<getNumElements(); i++)
        {
        getElement(i).setInUseState(newState);
        }
    }

size_t Base::sumAllExternalSize( void ) const
    {
    size_t   sum = 0;
    unsigned i;
    for(i=0; i<getNumElements(); i++)
        {
        sum += getElement(i).externalSize();
        }

    return sum;
    }

////////////////////////
void Api::copy( Api& dst, const Api& src, const Api* inUseFilterPtr ) 
    {
    // Mark the tuple state as: 'updated'
    dst.setUpdatedState();
    dst.incrementSequenceNumber();

    // DO the copy
    unsigned i;
    for(i=0; i < dst.getNumElements(); i++ )
        {
        // Only copy in-use elements (or all if specified)
        if ( !inUseFilterPtr || inUseFilterPtr->getElement(i).isInUse() )
            {
            dst.getElement(i).setValidFlagState( src.getElement(i).isValid() );
            dst.getElement(i).copyDataFrom( src.getElement(i) );
            }
        }
    }

bool Api::compareAndCopy( Rte::Tuple::Api& srcTuple, Rte::Tuple::Api& dstTuple, bool allElements, bool compareValues )
    {
    // Clear the 'destination tuple' (aka the tuple that might be updated) as not-updated
    dstTuple.clearUpdatedState();

    // Compare by sequence numbers
    bool tupleIsDifferent = false;
    if ( !compareValues )
        {
        if ( srcTuple.isDifferent( dstTuple ) )
            {
            tupleIsDifferent = true;
            }
        }
    
    // Do the compare by actual VALUES
    else
        {
        unsigned i;
        for(i=0; i < srcTuple.getNumElements(); i++ )
            {
            // Only check in-use elements (or all if specified)
            if ( dstTuple.getElement(i).isInUse() || allElements )
                {
                // Difference in the valid state
                if ( srcTuple.getElement(i).isValid() != dstTuple.getElement(i).isValid() )
                    {
                    tupleIsDifferent = true;
                    break;
                    }

                // Only compare data when it is VALID
                // NOTE: With respect to the value comparision: 'src' is the 
                //       point/tuple/element in the model, and 'dst' is the
                //       point/tuple/element of the model VIEWER.
                else if ( srcTuple.getElement(i).isValid() && dstTuple.getElement(i).isDifferentFrom(srcTuple.getElement(i)) )
                    {
                    tupleIsDifferent = true;
                    break;
                    }
                }
            }
        }

    // Update the destination tuple since it is different
    if ( tupleIsDifferent )
        {
        Rte::Tuple::Api::copy( dstTuple, srcTuple, allElements? 0: &dstTuple );
        dstTuple.setSequenceNumber( srcTuple.getSequenceNumber() );
        }

    return tupleIsDifferent;
    }
    
