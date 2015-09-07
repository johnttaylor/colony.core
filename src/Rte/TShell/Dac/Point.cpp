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

#include "Point.h"


///
using namespace Rte::TShell::Dac;



///////////////////////////////////
Point::Point( Cpl::Container::Map<Rte::TShell::Dac::Point>& pointList, Rte::Point::Model::Api& modelPoint, const char* symbolicName )
:m_mapKey(symbolicName)
,m_point(modelPoint)
    {
    pointList.insert( *this );
    }

Point::Point( Cpl::Container::Map<Rte::TShell::Dac::Point>& pointList, Rte::Point::Model::Api& modelPoint, const char* symbolicName, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance )
:Cpl::Container::MapItem(ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance)
,m_mapKey(symbolicName)
,m_point(modelPoint)
    {
    pointList.insert( *this );
    }


const Cpl::Container::Key& Point::getKey() const throw()
    {
    return m_mapKey;
    }

