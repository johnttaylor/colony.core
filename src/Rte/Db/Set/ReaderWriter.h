#ifndef Rte_Db_Set_ReaderWriter_h_
#define Rte_Db_Set_ReaderWriter_h_
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

#include "Rte/Db/Set/Base.h"
#include "Rte/Point/Controller/Base.h"
#include "Rte/Point/Viewer/Composer.h"


/// Namespace(s)
namespace Rte { namespace Db { namespace Set { 


/** This paritally concrete class provides the Controller and Viewer aspects 
    for a Set. The Application's concete Set class should inherit from this
    class.
 */
class ReaderWriter: public Rte::Db::Set::Base,
                    public Rte::Point::Controller::Base,
                    public Rte::Point::Viewer::Composer<ReaderWriter>
{
public:
    /// Constructor
    ReaderWriter( const char*                    setName,
                  Rte::Point::Api&               myPoint,
                  Cpl::Container::Map<ApiLocal>& mySetList,
                  Rte::Point::Model::Api&        modelPoint,
                  Cpl::Itc::PostApi&             setLayerMbox, 
                  Cpl::Timer::CounterSource&     timingSource,
                  HandlerLocal&                  setHandler, 
                  unsigned long                  delayWriteTimeInMsec = 0,    // Default is: no delay
                  Cpl::Log::Api&                 eventLogger          = Cpl::Log::Loggers::application()
                );


public: 
    /// See Rte::Db::Set::ApiPoint
    Rte::Point::Api& getPoint(void) throw();

    /// See Rte::Db::Set::ApiPoint
    Rte::Point::Viewer::Api& getViewer() throw();

    /// See Rte::Db::Set::ApiPoint
    Rte::Point::Controller::Api& getController() throw();

    /// See Rte::Db::Set::ApiPoint
    Rte::Point::Model::Api&  getModelPoint() throw();


protected:
    /// Helper (call back for viewer)
    void  pointChanged( void );

    /// Helper (call back for viewer)
    void  viewerStopped( void );
};


};      // end namespaces
};
};
#endif  // end header latch
