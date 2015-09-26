#ifndef Rte_Element_SeqNumber_h_
#define Rte_Element_SeqNumber_h_
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
#include <stdint.h>


///
namespace Rte { namespace Element {


/** This concrete class provides a sequence number data type.  
    The controller can set a less-than-zero, zero, or 
    greater-than-zero value.  The copyDataFrom() reacts
    differently based on the <,0,> value of the 'src' - see
    the table below:

        if src._data < 0,  then dst._data is increment
        if src._data == 0, then dst._data is NOT updated
        if src._data > 0,  then dst._data:= src._data
 */
class SeqNumber: public Base
{
protected:
    /// Storage for the sequence number
    int32_t m_data;

public:
    /// Constructor
    SeqNumber( bool   inUse      = false,
               int8_t validState = RTE_ELEMENT_API_STATE_INVALID
             );


public:
    /// See class description on what value to set
    virtual void set( int32_t newValue );

    /// Returns the current value
    virtual int32_t get(void) const;


public:
    /// See Rte::Element::Api
    bool copyDataFrom( const Api& other );

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

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    ///  See Rte::Element::Api (because of the sequence number semantics -->need to override Base class implemenation)
    const char* fromString( const char* srcText, const char* terminationChars=0, unsigned* updatedPtr=0 );

    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;
    

protected:
    /// See Rte::Element::Api
    const char* setFromText( const char* srcText, const char* terminationChars=0 );

    /// Helper method to apply the sequence number 'action'.  The caller is required to handle the case of 'operAndValue' == 0.
    virtual void applyNewValue( int32_t operAndValue );

};

};      // end namespaces
};
#endif  // end header latch

