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

#include "ReaderWriter.h"
#include "Cpl/System/Trace.h"

#define SECT_ "Rte::Db::Set"

///
using namespace Rte::Db::Set;

////////////////////////
ReaderWriter::ReaderWriter( const char*                    setName,
                            Rte::Point::Api&               myPoint,
                            Cpl::Container::Map<ApiLocal>& mySetList,
                            Rte::Point::Model::Api&        modelPoint,
                            Cpl::Itc::PostApi&             setLayerMbox, 
                            Cpl::Timer::CounterSource&     timingSource,
                            HandlerLocal&                  setHandler, 
                            unsigned long                  delayWriteTimeInMsec,
                            Cpl::Log::Api&                 eventLogger
                          )
:Rte::Db::Set::Base( mySetList, 
                     setHandler,   
                     delayWriteTimeInMsec, 
                     setName, 
                     setLayerMbox, 
                     timingSource, 
                     eventLogger 
                   )
,Rte::Point::Controller::Base( myPoint, modelPoint )
,Rte::Point::Viewer::Composer<ReaderWriter>::Composer( myPoint, 
                                                       *this, 
                                                       &ReaderWriter::pointChanged, 
                                                       &ReaderWriter::viewerStopped, 
                                                       modelPoint, 
                                                       setLayerMbox
                                                     )
        {
        // Set all of my Point's 'in-use' flags to true (since we will be reading/writing all tuples)
        myPoint.setAllInUseState(true);
        }


///////////////////////////////
Rte::Point::Api& ReaderWriter::getPoint(void) throw()                       
    { 
    return getMyPoint(); 
    }

Rte::Point::Viewer::Api& ReaderWriter::getViewer() throw()
    { 
    return *this; 
    }

Rte::Point::Controller::Api& ReaderWriter::getController() throw()
    { 
    return *this; 
    }

Rte::Point::Model::Api&  ReaderWriter::getModelPoint() throw()        
    { 
    return getModelPoint(); 
    }



/////////////////////////////////////////
void  ReaderWriter::pointChanged( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("ReaderWriter::pointChanged [%s]", m_name()) );
    sendEvent( evDataModified );
    }

void  ReaderWriter::viewerStopped( void )
    {
    CPL_SYSTEM_TRACE_MSG( SECT_, ("ReaderWriter::viewerStopped [%s]", m_name()) );
    sendEvent( evDisconnected );
    }


