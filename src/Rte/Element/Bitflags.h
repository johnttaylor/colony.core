#ifndef Rte_Element_Bitflags_h_
#define Rte_Element_Bitflags_h_
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


/** This concrete class provides a bit flags data type where the setting, 
    clearing, and toggling individual bits (i.e. the read/modify/write 
    operation) is done in the Model thread.  There are 29 usable bits.

    Bit lay out:
               24       16      8
        7654321 76543210 7654321 76543210
        sctBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
        s := set bits operator
        c := clear bits operator
        t := toggle bits operator
        B := usable bits


    A controller select the type of 'operation' to be performed on the bit flags 
    based on the value it is writing to the model/element.  The copyDataFrom() 
    reacts differently based on the value of the 'src' - see the table below:

        if src._data >= 0xC0000000, then dst._bits are NOT updated
        if 0x80000000 <= src._data <= 0xBFFFFFF  then dst._bits are set
        if 0x40000000 <= src._data <= 0x7FFFFFF  then dst._bits are cleared
        if 0x20000000 <= src._data <= 0x3FFFFFF  then dst._bits are toggled
        if 0 <= src._data <= 0x1FFFFFFF,  then dst._bits:= src._bits
 */

class BitFlags: public Base
{
public:
    /// Bit Operators and/or masks
    enum { eUSABLE_BITS_MASK = 0x1FFFFFFF,  // Mask for the 'usable' bits
           eNO_UPDATE        = 0xFFFFFFFF,  // Write value to perform a 'NOP' when updating the model
           eOPER_SET_BITS    = 0x80000000,  // Set bit operator 
           eOPER_CLR_BITS    = 0x40000000,  // Clear bit operator
           eOPER_TGL_BITS    = 0x20000000   // Toggle bit operator
         };

protected:
    /// Storage for the bit flags
    uint32_t m_data;


public:
    /// Constructor
    BitFlags( bool   inUse      = false,
              int8_t validState = RTE_ELEMENT_API_STATE_INVALID
            );


public:
    /// See class description on what value to set
    virtual void set( uint32_t newValue );

    /// Returns the current value
    virtual uint32_t get(void) const;


public:
    /// Set Bit operation
    inline void setBits( uint32_t bitsToSet )       { set( (bitsToSet&eUSABLE_BITS_MASK)|eOPER_SET_BITS ); }

    /// Clear Bit operation
    inline void clearBits( uint32_t bitsToClear )   { set( (bitsToClear&eUSABLE_BITS_MASK)|eOPER_CLR_BITS ); } 
    
    /// Toggle Bit operation
    inline void toggleBits( uint32_t bitsToToggle ) { set( (bitsToToggle&eUSABLE_BITS_MASK)|eOPER_TGL_BITS ); }
              

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
    const char* toString( Cpl::Text::String& dstMemory ) const;
    
    /** See Rte::Element::Api. Input format is: Numeric number in base 10, 
        or base 8 when a leading zero is specified, or base 16 if a leading
        '0x' (or '0X') is specified.
     */
    bool setFromText( const char* srcText );

};

};      // end namespaces
};
#endif  // end header latch

