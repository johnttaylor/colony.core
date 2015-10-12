#ifndef Tuple_Foo2_h_
#define Tuple_Foo2_h_
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


#include "Rte/Tuple/Basic.h"
#include "Rte/Element/BitFlags.h"
#include "Rte/Element/SeqNumber.h"
#include "Rte/Element/RefCounter.h"
#include "Rte/Element/Delta.h"
#include "Rte/Element/Md5.h"

namespace Tuple {


/** Concrete Tuple: FOO2
 */
class Foo2: public Rte::Tuple::Basic<5>
{
public: // Provide the domain application access to the Data
    ///
    Rte::Element::BitFlags      m_flags;
    ///
    Rte::Element::SeqNumber     m_seqNum;
    ///
    Rte::Element::TimeTDelta_T  m_time;
    ///
    Rte::Element::Md5           m_checksum;
    ///
    Rte::Element::RefCounter    m_refCnt;

public:
    /// Constructor
    Foo2( void )
    :m_time(60)   // Set delta to 1 minute
        {
        registerElement( 0, m_flags );
        registerElement( 1, m_seqNum );
        registerElement( 2, m_time );
        registerElement( 3, m_checksum );
        registerElement( 4, m_refCnt );
        }
};

};
#endif
