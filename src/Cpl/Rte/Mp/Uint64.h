#ifndef Cpl_Rte_Mp_Uint64_h_
#define Cpl_Rte_Mp_Uint64_h_
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
/** @file */


#include "Cpl/Rte/Mp/Basic.h"

///
namespace Cpl {
///
namespace Rte {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    uint64_t.

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Uint64 : public Basic<uint64_t>
{
protected:
    /// Flag for to/from string() methods
    bool m_decimal;

public:
    /// Constructor
    Uint64( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, bool decimalFormat=true, int8_t validState = OPTION_CPL_RTE_MODEL_POINT_STATE_INVALID, uint64_t initialValue = 0 );

public:
    /// Type safe read. See Cpl::Rte::ModelPoint
    virtual uint16_t read( uint64_t& dstData, int8_t& validState ) const throw();

    /// Type safe write. See Cpl::Rte::ModelPoint
    virtual uint16_t write( uint64_t newValue, LockRequest_T lockRequest = eNO_REQUEST ) throw();

    /// Type safe read-modify-write client callback interface
    typedef Cpl::Rte::ModelPointRmwCallback<uint64_t> Client;

    /** Type safe read-modify-write. See Cpl::Rte::ModelPoint

       NOTE: THE USE OF THIS METHOD IS STRONGLY DISCOURAGED because it has
             potential to lockout access to the ENTIRE Model Base for an
             indeterminate amount of time.  And alternative is to have the
             concrete Model Point leaf classes provide the application
             specific read, write, read-modify-write methods in addition or in
             lieu of the read/write methods in this interface.
     */
    virtual uint16_t readModifyWrite( Client& callbackClient, LockRequest_T lockRequest = eNO_REQUEST );


public:
    /// Type safe subscriber
    typedef Cpl::Rte::Subscriber<Uint64> Observer;

    /// Type safe register observer
    virtual void attach( Observer& observer, uint16_t initialSeqNumber=SEQUENCE_NUMBER_UNKNOWN ) throw();

    /// Type safe un-register observer
    virtual void detach( Observer& observer ) throw();


public:
    ///  See Cpl::Rte::ModelPoint.
    bool toString( Cpl::Text::String& dst, bool append=false, uint16_t* retSequenceNumber=0 ) const throw();

    ///  See Cpl::Rte::ModelPoint.
    const char* getTypeAsText() const throw();

protected:
    /// See Cpl::Rte::ModelPointCommon_.
    const char* setFromText( const char* srcText, LockRequest_T lockAction, const char* terminationChars=0, Cpl::Text::String* errorMsg=0, uint16_t* retSequenceNumber=0 ) throw();
};



};      // end namespaces
};
};
#endif  // end header latch
