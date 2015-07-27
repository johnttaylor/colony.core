#ifndef Sets_h_
#define Sets_h_
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

#include "Rte/Db/Set/ReaderWriter.h"
#include "Rte/Db/Set/ContainerReaderWriter.h"
#include "Points.h"



/** Concrete Set: BAR1
 */
class SetBar1: public PointBar1,
               public Rte::Db::Set::ReaderWriter
{
public:
    /// Constructor
    SetBar1( Cpl::Container::Map<ApiLocal>& mySetList,
             ModelBar1&                     modelPoint,
             Cpl::Itc::PostApi&             setLayerMbox, 
             Cpl::Timer::CounterSource&     timingSource,
             Rte::Db::Set::HandlerLocal&    setHandler, 
             unsigned long                  delayWriteTimeInMsec = 0,    // Default is: no delay
             Cpl::Log::Api&                 eventLogger = Cpl::Log::Loggers::application()
           )
    :Rte::Db::Set::ReaderWriter( "SetBar1",
                                 *this,
                                 mySetList, 
                                 modelPoint,
                                 setLayerMbox, 
                                 timingSource, 
                                 setHandler,
                                 delayWriteTimeInMsec, 
                                 eventLogger 
                               )
        {
        }



public: /// See Rte::Db::Set::ApiPoint
    ///
    void defaultSet( void ) throw()
        {
        // Default ALL tuples/elements to the VALID state 
        PointBar1::setAllValidState( RTE_ELEMENT_API_STATE_VALID );

        // Default Tuple1
        m_fields1.m_flag.set( true );
        m_fields1.m_text.set( "<description>" );
        m_fields1.m_data1.set( 0 );
        m_fields1.m_data2.setInvalid();  // Example of optional Element
        m_fields1.m_enum.set( 2 );
    
        // Default Tuple2
        m_fields2.m_text.set( "<label>" );
        }

};



/** Concrete Set: BAR2
 */
class SetBar2: public PointBar2,
               public Rte::Db::Set::ContainerReaderWriter
{
public:
    /// Constructor
    SetBar2( Cpl::Container::Map<ApiLocal>& mySetList,
             ModelBar2&                     modelPoint,
             Cpl::Itc::PostApi&             setLayerMbox, 
             Cpl::Timer::CounterSource&     timingSource,
             Rte::Db::Set::HandlerLocal&    setHandler, 
             unsigned long                  delayWriteTimeInMsec = 0,    // Default is: no delay
             Cpl::Log::Api&                 eventLogger = Cpl::Log::Loggers::application()
           )
    :Rte::Db::Set::ContainerReaderWriter( "SetBar2",
                                          *this,
                                          mySetList, 
                                          modelPoint,
                                          setLayerMbox, 
                                          timingSource, 
                                          setHandler,
                                          delayWriteTimeInMsec, 
                                          eventLogger 
                                        )
        {
        }


public: /// See Rte::Db::Set::ApiPoint
    ///
    void defaultSet( void ) throw()
        {
        // Default ALL elements (in ALL tuples) to INVALID
        PointBar2::setAllValidState( RTE_ELEMENT_API_STATE_INVALID );

        // Default to only 1 (of 4) Tuples being 'valid/existing'
        removeItem(0);
        removeItem(1);
        addItem(2);
        removeItem(3);

        // Default the '1' valid instance
        m_tuples_[2].setAllValidState(RTE_ELEMENT_API_STATE_VALID);
        m_tuples_[2].m_flag.set( true );
        m_tuples_[2].m_text.set( "<name>" );
        m_tuples_[2].m_data.set( 23 );
        }

};




#endif 