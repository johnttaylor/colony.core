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

#include "ModelDatabase.h"
#include "ModelPoint.h"
#include "Cpl/Container/Key.h"

///
using namespace Cpl::Rte;

ModelDatabase::ModelDatabase() throw()
    : Cpl::Container::Map<ModelPoint>()
{
}

ModelDatabase::ModelDatabase(const char* ignoreThisParameter_usedToCreateAUniqueConstructor) throw()
    : Cpl::Container::Map<ModelPoint>(ignoreThisParameter_usedToCreateAUniqueConstructor)
{
}

ModelPoint * ModelDatabase::lookupModelPoint( const char* modelPointName ) const throw()
{
    Cpl::Container::KeyLiteralString myKey( modelPointName );
    return find( myKey );
}
