#ifndef Rte_Element_Md5_h_
#define Rte_Element_Md5_h_
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

#include "Rte/Element/Base.h"
#include "Cpl/Checksum/ApiMd5.h"


///
namespace Rte { namespace Element {


/** This concrete class provides an implemenation for a Element
    that has data content a MD5 Checksum.
 */
class Md5: public Base
{
protected:
    ///
    Cpl::Checksum::ApiMd5::Digest_T m_data;


public:
    /// Constructor
    Md5( bool inUse      = false,
         bool validFlag  = false
       );


public:
    /// Updates the data content's value
    virtual void set( const Cpl::Checksum::ApiMd5::Digest_T& newValue );

    /// Returns the data content's value
    virtual Cpl::Checksum::ApiMd5::Digest_T& get(void);


public: 
    /// See Rte::Element::Api
    void copyDataFrom( const Api& other );

    /// See Rte::Element::Api
    bool isDifferentFrom( const Api& other ) const;

    /// See Rte::Element::Api
    const void* dataPointer( void ) const;

    /// See Rte::Element::Api
    size_t exportElement( void* dstDataStream ) const;

    /// See Rte::Element::Api
    size_t importElement( const void* srcDataStream );

    /// See Rte::Element::Api
    size_t externalSize(void) const;
};

};      // end namespaces
};
#endif  // end header latch

