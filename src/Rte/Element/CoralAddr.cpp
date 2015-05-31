/*----------------------------------------------------------------------------- 
* This file is part of the Shift-Right Open Repository.  The Repository is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the root directory or on the Internet at           
* http://www.shift-right.com/openrepo/license.htm                               
*                                                                               
* Copyright (c) 2001-2011 John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 

#include "CoralAddr.h"


///
using namespace Rte::Element;


//////////////////////////////////
Rte::Element::CoralAddr::CoralAddr( const char*  initialValue,
                                    bool         inUse,
                                    bool         validFlag
                                  )
//:Rte::Element::String<OPTION_CORAL_SZ_ADDRESS>(DataType::CORAL_ADDR,initialValue,inUse,validFlag)
:Rte::Element::String<10>(DataType::CORAL_ADDR,initialValue,inUse,validFlag)
    {
    }



