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
#include "Cpl/System/Trace.h"


///
using namespace Rte::Tuple;

#define SECT_   "Rte::Tuple"


///////////////////////
Base::Base( void )
:m_seqnum_(1)           // Initial the sequence number something 'valid' (but this requires all viewers to invalidate their sequence numbers during construction)
,m_updated_(false)
    {
    }

/////////////////
bool Base::isUpdated( void ) const
    {
    return m_updated_;
    }

void Base::clearUpdatedState( void )
    {
    m_updated_ = false;
    }

void Base::setUpdatedState( void )
    {
    m_updated_ = true;
    }


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

void Base::invalidateSequenceNumber(void)
    {
    m_seqnum_ = 0;
    }

bool Base::isDifferent( Api& other ) const
    {
    if ( m_seqnum_ == 0 || other.getSequenceNumber() == 0 || m_seqnum_ != other.getSequenceNumber() )
        {
        return true;
        }

    return false;
    }


/////////////////
void Base::setAllValidState( int8_t newState ) 
    {
    unsigned i;
    for(i=0; i<getNumElements(); i++)
        {
        getElement(i).setValidState(newState);
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

void Base::setAsModelTuple_(void)
    {
    unsigned i;
    for(i=0; i<getNumElements(); i++)
        {
        getElement(i).setAsModelElement_();
        }
    }

void Base::setAllLockedState( bool newState )
    {
    unsigned i;
    for(i=0; i<getNumElements(); i++)
        {
        getElement(i).setLockedState( newState );
        }
    }

void Base::requestAllLockOperation( bool newState )
    {
    unsigned i;
    for(i=0; i<getNumElements(); i++)
        {
        getElement(i).requestLockOperation( newState );
        }
    }



////////////////////////
bool Api::copy( Api& dst, const Api& src, const Api* inUseFilterPtr ) 
    {
    unsigned updated = 0;

    // Skip copy if the source Tuple is a Null Tuple
    if ( src.getNumElements() != 0 )
        {
        // DO the copy
        unsigned i;
        for(i=0; i < dst.getNumElements(); i++ )
            {
            // Only copy in-use elements (or all if specified)
            if ( !inUseFilterPtr || inUseFilterPtr->getElement(i).isInUse() )
                {
                if ( dst.getElement(i).copyDataFrom( src.getElement(i) ) )
                    {
                    updated++;
                    dst.getElement(i).setRawValidState_( src.getElement(i).getRawValidState_() ); 
                    }
                }

            // Apply lock operations (if any).  MUST be done AFTER the copy operations (so that new value can be applied with a lock request)!
            if ( inUseFilterPtr )
                {
                if ( inUseFilterPtr->getElement(i).isLockRequest() )
                    {
                    // Treat a change in the lock state as an update
                    if ( dst.getElement(i).isLocked() == false )
                        {
                        updated++;      
                        }

                    // update the lock state
                    dst.getElement(i).setLocked();

                    // Clear lock request -->treat it as a 'one-shot' operation
                    inUseFilterPtr->getElement(i).clearLockOperation();

                    }
                else if ( inUseFilterPtr->getElement(i).isUnlockRequest() )
                    {
                    // Treat a change in the lock state as an update
                    if ( dst.getElement(i).isLocked() == true )
                        {
                        updated++;      
                        }

                    // update the lock state
                    dst.getElement(i).setUnlocked();

                    // Clear unlock request -->treat it as a 'one-shot' operation
                    inUseFilterPtr->getElement(i).clearLockOperation();
                    } 
                }

            }
        }

    // Mark the tuple state as: 'updated'
    if ( updated )
        {
        dst.setUpdatedState();
        dst.incrementSequenceNumber();
        return true;
        }

    return false;
    }

bool Api::compareAndCopy( Rte::Tuple::Api& srcTuple, Rte::Tuple::Api& dstTuple, bool allElements, bool compareValues )
    {
    // Clear the 'destination tuple' (aka the tuple that might be updated) as not-updated
    dstTuple.clearUpdatedState();

    // Compare by sequence numbers (OR force a sequence number comparsion if my destination Tuple is a Null Tuple)
    bool tupleIsDifferent = false;
    if ( !compareValues || dstTuple.getNumElements() == 0 )
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Compare by SEQNUM (dst.numElems=%u, src.numElems=%u, dst.seq=%u, src.seq=%u)", dstTuple.getNumElements(), srcTuple.getNumElements(), dstTuple.getSequenceNumber(), srcTuple.getSequenceNumber() ));
        if ( srcTuple.isDifferent( dstTuple ) )
            {
            tupleIsDifferent = true;
            }
        }
    
    // Do the compare by actual VALUES
    else
        {
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Compare by VALUE (dst.numElems=%u, src.numElems=%u, dst.seq=%u, src.seq=%u)", dstTuple.getNumElements(), srcTuple.getNumElements(), dstTuple.getSequenceNumber(), srcTuple.getSequenceNumber() ));

        unsigned i;
        for(i=0; i < srcTuple.getNumElements(); i++ )
            {
            // Only check in-use elements (or all if specified)
            if ( dstTuple.getElement(i).isInUse() || allElements )
                {
                // Difference in the valid state
                if ( srcTuple.getElement(i).getRawValidState_() != dstTuple.getElement(i).getRawValidState_() )
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
    
