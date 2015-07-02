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


#include "Null.h"


///
using namespace Rte::Db::Chunk;


//////////////////////////////
Null::Null()
    {
    }


//////////////////////////////
Cpl::Io::File::InputOutputApi* Null::openDatabase( bool& newfile ) throw()
    {
    newfile = true;
    return this;
    }


void Null::closeDatabase() throw()
    {
    }


bool Null::deleteDatabase() throw()
    {
    return true;
    }


