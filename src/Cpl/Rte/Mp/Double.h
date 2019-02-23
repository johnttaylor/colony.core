#ifndef Cpl_Rte_Mp_Double_h_
#define Cpl_Rte_Mp_Double_h_
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


#include "Cpl/Rte/Mp/Basic.h"

///
namespace Cpl {
///
namespace Rte {
///
namespace Mp {


/** This class provides a concrete implementation for a Point who's data is a
    double.

    NOTE: All methods in this class ARE thread Safe unless explicitly
          documented otherwise.
 */
class Double : public Basic<double>
{
public:
    /// Constructor. Invalid MP. 
    Double( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo );

    /// Constructor. Valid MP.  Requires an initial value
    Double( Cpl::Rte::ModelDatabase& myModelBase, StaticInfo& staticInfo, double initialValue  );

public:
    /// Type safe read. See Cpl::Rte::ModelPoint
    virtual int8_t read( double& dstData, uint16_t* seqNumPtr=0 ) const throw();

    /// Type safe write. See Cpl::Rte::ModelPoint
    virtual uint16_t write( double newValue, LockRequest_T lockRequest = eNO_REQUEST ) throw();

    /// Type safe read-modify-write client callback interface
    typedef Cpl::Rte::ModelPointRmwCallback<double> Client;

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
    typedef Cpl::Rte::Subscriber<Double> Observer;

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

    /// See Cpl::Rte::ModelPoint.  Note: Use the system wide default epsilon of CPL_MATH_REAL_FLOAT_EPSILON when testing for equality
    bool isDataEqual_( const void* otherData ) const throw();

};



};      // end namespaces
};
};
#endif  // end header latch
