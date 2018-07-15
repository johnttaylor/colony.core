#ifndef Rte_TShell_Dac_Point_h_
#define Rte_TShell_Dac_Point_h_
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

#include "Cpl/Container/MapItem.h"
#include "Cpl/Container/Key.h"
#include "Cpl/Container/Map.h"
#include "Rte/Point/Model/Api.h"


///
namespace Rte { namespace TShell { namespace Dac {


/** This class associates a Model Point with a Symbolic Name
 */
class Point: public Cpl::Container::MapItem
{
protected:
    /// Point name 
    Cpl::Container::KeyLiteralString  m_mapKey;

    /// Model Point
    Rte::Point::Model::Api&           m_point;


public:
    /// Constructor
    Point( Cpl::Container::Map<Rte::TShell::Dac::Point>& pointList, Rte::Point::Model::Api& modelPoint, const char* symbolicName );

    /// Constructor (for creating a static instance)
    Point( Cpl::Container::Map<Rte::TShell::Dac::Point>& pointList, Rte::Point::Model::Api& modelPoint, const char* symbolicName, const char* ignoreThisParameter_onlyUsedWhenCreatingAStaticInstance );


public:
    /// This method Symbolic name for the Model Point
    inline const char* getName() const throw()  { return m_mapKey; }


    /// This method a reference to the Model Point
    inline Rte::Point::Model::Api& getModelPoint() const throw() { return m_point; }


protected:
    /// See Cpl::Container::Key
    const Cpl::Container::Key& getKey() const throw();

};

};      // end namespaces
};
};
#endif  // end header latch
