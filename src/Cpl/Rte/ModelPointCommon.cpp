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

#include "ModelPointCommon.h"

///
using namespace Cpl::Rte;


////////////////////////
ModelPointCommon::ModelPointCommon( ModelBase& myModelBase, Point& myData, StaticInfo* staticInfo, bool isValid )
    : m_staticInfo( staticInfo )
    , m_modelBase( myModelBase )
    , m_data( myData )
    , m_seqNum( SEQUENCE_NUMBER_UNKNOW )
    , m_valid( isValid )
    , m_forceLevel( 0 )
{
}

/////////////////
const char* ModelPointCommon::getName() const throw()
{
    return m_staticInfo->getName();
}

uint16_t ModelPointCommon::getSequenceNumber() const throw()
{
    m_modelBase.lock();
    uint16_t result = m_seqNum;
    m_modelBase.unlock();
    return result;
}

bool ModelPointCommon::isValid() const throw()
{
    m_modelBase.lock();
    bool result = m_valid;
    m_modelBase.unlock();
    return result;
}

uint16_t ModelPointCommon::read( Point& dst, bool& isValid ) const throw()
{
    m_modelBase.lock();
    isValid = m_valid;
    if ( isValid )
    {
        dst.copyFrom( m_data );
    }
    uint16_t result = m_seqNum;
    m_modelBase.unlock();

    return result;
}

uint16_t ModelPointCommon::write( const Point& src, Force_T forceLevel ) throw()
{
    m_modelBase.lock();
    if ( testAndSetForceLevel( forceLevel ) )
    {
        if ( !m_valid || m_data.isEqual( src ) == false )
        {
            m_data.copyFrom( src );
            processDataChanged();
        }
    }
    uint16_t result = m_seqNum;
    m_modelBase.unlock();

    return result;
}

uint16_t ModelPointCommon::readModifyWrite( RmwCallbackApi& callbackClient, Force_T forceLevel )
{
    m_modelBase.lock();
    if ( testAndSetForceLevel( forceLevel ) )
    {
        // Invoke the client's callback function
        RmwCallbackApi::Result_T result = callbackClient.modelPointRmwCallback( m_data, m_valid );

        // Do nothing if the callback did not change anything
        if ( result != RmwCallbackApi::eNO_CHANGE )
        {
            // Handle request to invalidate the MP data
            if ( result == RmwCallbackApi::eINVALIDATE )
            {
                if ( m_valid )
                {
                    m_valid = false;
                    processDataChanged();
                }
            }

            // Handle the CHANGED use case
            else
            {
                processDataChanged();
            }
        }

    }

    uint16_t result = m_seqNum;
    m_modelBase.unlock();

    return result;
}

uint16_t ModelPointCommon::touch() throw()
{
    m_modelBase.lock();
    processDataChanged();
    uint16_t result = m_seqNum;
    m_modelBase.unlock();
    return result;
}

uint16_t ModelPointCommon::setInvalid() throw()
{
    m_modelBase.lock();
    if ( m_valid )
    {
        m_valid = false;
        processDataChanged();
    }

    uint16_t result = m_seqNum;
    m_modelBase.unlock();
    return result;
}

/////////////////
void ModelPointCommon::removeAllForceLevels() throw()
{
    m_modelBase.lock();
    m_forceLevel = 0;
    m_modelBase.unlock();
}

void ModelPointCommon::removeForceLevel( Force_T forceLevelToRemove ) throw()
{
    if ( forceLevelToRemove != eNOT_FORCED )
    {
        uint8_t bitMask = 1 << (forceLevelToRemove - 1);
        m_modelBase.lock();
        m_forceLevel &= ~bitMask;
        m_modelBase.unlock();
    }
}

uint16_t ModelPointCommon::removeForceLevel( Force_T forceLevelToRemove, const Point& src ) throw()
{
    m_modelBase.lock();
    if ( testAndClearForceLevel( forceLevelToRemove ) )
    {
        if ( !m_valid || m_data.isEqual( src ) == false )
        {
            m_data.copyFrom( src );
            processDataChanged();
        }
    }
    uint16_t result = m_seqNum;
    m_modelBase.unlock();

    return result;
}


/////////////////
int ModelPointCommon::compareKey( const Key& key ) const
{
    return m_staticInfo->compareKey( key );
}

const void* ModelPointCommon::getRawKey( unsigned* returnRawKeyLenPtr ) const
{
    return m_staticInfo->getRawKey( returnRawKeyLenPtr );
}

/////////////////
void ModelPointCommon::processDataChanged() throw()
{
    m_valid = true;
}

bool ModelPointCommon::testAndSetForceLevel( Force_T forceLevel ) throw()
{
    bool result = false;
    if ( m_forceLevel == 0 )
    {
        result = true;
        if ( forceLevel != eNOT_FORCED )
        {
            m_forceLevel = 1 << (forceLevel - 1);
        }
    }
    else
    {
        // Check if the new force level is sufficient
        if ( forceLevel >= getHighestForceLevel() )
        {
            result        = true;
            m_forceLevel |= 1 << (forceLevel - 1);
        }
    }

    return result;
}

bool ModelPointCommon::testAndClearForceLevel( Force_T forceLevel ) throw()
{
    bool result = false;
    if ( m_forceLevel == 0 )
    {
        result = true;
    }
    else
    {
        // Check if the new force level is sufficient
        if ( forceLevel >= getHighestForceLevel() )
        {
            result = true;
        }
    }

    // Clear the force level
    uint8_t bitMask = 1 << (forceLevel - 1);
    m_forceLevel &= ~bitMask;

    return result;
}

ModelPoint::Force_T ModelPointCommon::getHighestForceLevel() const throw()
{
    // Find the highest force level that is current set
    unsigned mask;
    int      idx;
    for ( mask=0x80, idx=8; idx > 0; idx--, mask >>= 1 )
    {
        if ( (m_forceLevel & mask) == mask )
        {
            return (Force_T) idx;
        }
    }

    return eNOT_FORCED;
}
