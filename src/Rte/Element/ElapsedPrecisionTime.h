#ifndef Rte_Element_Precision_h_
#define Rte_Element_Precision_h_
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

#include "Rte/Element/Basic.h"
#include "Cpl/System/ElapsedTime.h"


///
namespace Rte { namespace Element {



/** This class provides an RTE Element for the Cpl::System::ElapsedTime::Precision_T
    data type
 */
class ElapsedPrecisionTime: public Basic<Cpl::System::ElapsedTime::Precision_T,DataType::ELAPSED_PRECISION_TIME>
{
public:
    /// Constructor
    ElapsedPrecisionTime( bool    inUse        = false,
                          int8_t  validState   = RTE_ELEMENT_API_STATE_INVALID
                        );


public:
    /// See Rte::Element::Api
    const char* getTypeAsText(void) const;

    /// See Rte::Element::Api
    const char* toString( Cpl::Text::String& dstMemory, bool append=false ) const;
    
    
protected:
    /// See Rte::Element::Api.  Format: "T"=true, "F"=false
    const char* setFromText( const char* srcText, const char* terminationChars=0 );
    
};



};      // end namespaces
};
#endif  // end header latch

