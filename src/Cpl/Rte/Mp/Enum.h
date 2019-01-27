#ifndef Cpl_Rte_Mp_Enum_h_
#define Cpl_Rte_Mp_Enum_h_
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


#include "Cpl/Rte/ModelPointCommon_.h"
#include "Cpl/System/Assert.h"

///
namespace Cpl {
///
namespace Rte {
///
namespace Mp {


/** This class provides a MOSTLY concrete implementation for a Point who's data
    is a an a "Better Enum", a.k.a a "enum" defines using the BETTER_ENUM macro
    defined in Cpl/Type/enum.h.  A child class is still needed to provide the
    following methods:
        getTypeAsText()
        attach()
        detach()

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
template<class BETTERENUM_TYPE>
class Enum : public Cpl::Rte::ModelPointCommon_
{
protected:
    /// The element's value
    BETTERENUM_TYPE    m_data;

public:
    /// Constructor. 
    Enum( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID )
        , m_data( BETTERENUM_TYPE::_values()[0] )
    {
    }

    /// Constructor. 
    Enum( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, BETTERENUM_TYPE initialValue )
        :ModelPointCommon_( myModelBase, &m_data, staticInfo, Cpl::Rte::ModelPoint::MODEL_POINT_STATE_VALID )
        , m_data( initialValue )
    {
    }


public:
    /// Type safe read. See Cpl::Rte::ModelPoint
    virtual uint16_t read( BETTERENUM_TYPE& dstData, int8_t& validState ) const throw()
    {
        return ModelPointCommon_::read( &dstData, sizeof( int ), validState );
    }

    /// Type safe write. See Cpl::Rte::ModelPoint
    virtual uint16_t write( BETTERENUM_TYPE newValue, LockRequest_T lockRequest = eNO_REQUEST ) throw()
    {
        return ModelPointCommon_::write( &newValue, sizeof( int ), lockRequest );
    }

    /// Type safe read-modify-write client callback interface
    typedef Cpl::Rte::ModelPointRmwCallback<BETTERENUM_TYPE> Client;

    /** Type safe read-modify-write. See Cpl::Rte::ModelPoint

       NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
             potential to lockout access to the ENTIRE Model Base for an
             indeterminate amount of time.  And alternative is to have the
             concrete Model Point leaf classes provide the application
             specific read, write, read-modify-write methods in addition or in
             lieu of the read/write methods in this interface.
     */
    virtual uint16_t readModifyWrite( Client& callbackClient, LockRequest_T lockRequest = eNO_REQUEST )
    {
        return ModelPointCommon_::readModifyWrite( callbackClient, lockRequest );
    }



public:
    /// See Cpl::Rte::ModelPoint.  This method IS thread safe.
    size_t getSize() const throw()
    {
        return sizeof( BETTERENUM_TYPE );
    }

public:
    /// See Cpl::Rte::ModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const throw()
    {
        CPL_SYSTEM_ASSERT( dstSize == sizeof( BETTERENUM_TYPE ) );
        *((BETTERENUM_TYPE*) dstData) = m_data;
    }

    /// See Cpl::Rte::ModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) throw()
    {
        CPL_SYSTEM_ASSERT( srcSize == sizeof( BETTERENUM_TYPE ) );
        m_data = *((BETTERENUM_TYPE*) srcData);
    }

    /// See Cpl::Rte::ModelPoint.  The default implementation is for integers
    bool isDataEqual_( const void* otherData ) const throw()
    {
        BETTERENUM_TYPE left = *((BETTERENUM_TYPE*) otherData);
        return m_data == left;
    }

    /// See Cpl::Rte::Point.  
    const void* getImportExportDataPointer_() const throw()
    {
        return (const void*) (&m_data);
    }

    /// See Cpl::Rte::Point.  
    size_t getImportExportSize_() const throw()
    {
        return sizeof( BETTERENUM_TYPE );
    }


public:
    ///  See Cpl::Rte::ModelPoint.
    bool toString( Cpl::Text::String& dst, bool append=false, uint16_t* retSequenceNumber=0 ) const throw()
    {
        // Get a snapshot of the my data and state
        m_modelDatabase.lock_();
        BETTERENUM_TYPE  value  = m_data;
        uint16_t seqnum = m_seqNum;
        int8_t   valid  = m_validState;
        bool     locked = m_locked;
        m_modelDatabase.unlock_();

       // Convert data and state to a string
        if ( convertStateToText( dst, append, locked, valid ) )
        {
            // Convert data and state to a string
            dst.formatOpt( append, value._to_string() );
        }

        // Return the sequence number when requested
        if ( retSequenceNumber )
        {
            *retSequenceNumber = seqnum;
        }

        return true;
    }

protected:
    /// See Cpl::Rte::ModelPointCommon_.
    const char* setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars=0, Cpl::Text::String* errorMsg=0, uint16_t* retSequenceNumber=0 ) throw()
    {
        const char*   result = 0;
        uint16_t      seqnum = SEQUENCE_NUMBER_UNKNOWN;
        char*         srcPtr = (char*) srcText;


        // Make a temporary copy of source text when there are termination characters
        ModelDatabase::globalLock_();
        if ( terminationChars )
        {
            size_t idx = 0;
            srcPtr     = ModelDatabase::g_parseBuffer_;
            result     = srcText;
            while ( srcText[idx] != '\0' && strchr( terminationChars, srcText[idx] ) == 0 && idx < (sizeof( ModelDatabase::g_parseBuffer_ ) - 1) )
            {
                srcPtr[idx] = srcText[idx];
                idx++;
                result++;
            }

            // Null terminate the temp copy
            srcPtr[idx] = '\0';
        }

        // No termination characters -->get a reference to the end of the string token (aka the methods return value)
        else
        {
            result = srcText + strlen( srcText );
        }

        // Convert the text to an enum value
        auto maybeValue = BETTERENUM_TYPE::_from_string_nothrow( srcPtr );
        ModelDatabase::globalUnlock_();

        // Successful conversion
        if ( maybeValue )
        {
            seqnum = write( *maybeValue, lockAction );
        }

        // Conversion failed!
        else
        {
            // Set conversion failed
            result = 0;

            if ( errorMsg )
            {
                errorMsg->format( "Conversion of %s [%s] to a 'enum' failed", getTypeAsText(), srcText );
            }
        }

        // Housekeeping
        if ( retSequenceNumber )
        {
            *retSequenceNumber = seqnum;
        }

        return result;
    }
};



};      // end namespaces
};
};
#endif  // end header latch
