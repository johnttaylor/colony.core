#ifndef Records_h_
#define Records_h_
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

#include "Rte/Db/Record/Base.h"
#include "Points.h"



/** Concrete Record: BAR1
 */
class RecordBar1: public ModelBar1,
                  public Rte::Db::Record::Base
{
public:
    ///
    PointBar1Null   m_nullPoint4Viewer;

public:
    /// Constructor
    RecordBar1( Cpl::Container::Map<ApiLocal>& myRecordList,
                Cpl::Itc::PostApi&             recordLayerMbox, 
                Cpl::Timer::CounterSource&     timingSource,
                unsigned long                  delayWriteTimeInMsec = 0,    // Default is: no delay
                Cpl::Log::Api&                 eventLogger = Cpl::Log::Loggers::application()
              )
    :ModelBar1(recordLayerMbox)
    ,Rte::Db::Record::Base( m_nullPoint4Viewer,
                            *this,
                            myRecordList, 
                            delayWriteTimeInMsec, 
                            "RecordBar1",
                            recordLayerMbox, 
                            timingSource, 
                            eventLogger 
                          )
        {
        }


public: 
    /// See Rte::Point::Model::Api
    void defaultMe( void ) throw()
        {
        // Default ALL tuples/elements to the VALID state 
        setAllValidState( RTE_ELEMENT_API_STATE_VALID );

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



/** Concrete Record: BAR2
 */
class RecordBar2: public ModelBar2,
                  public Rte::Db::Record::Base

{
public:
    ///
    PointBar2Null   m_nullPoint4Viewer;

public:
    /// Constructor
    RecordBar2( Cpl::Container::Map<ApiLocal>& myRecordList,
                Cpl::Itc::PostApi&             recordLayerMbox, 
                Cpl::Timer::CounterSource&     timingSource,
                unsigned long                  delayWriteTimeInMsec = 0,    // Default is: no delay
                Cpl::Log::Api&                 eventLogger = Cpl::Log::Loggers::application()
              )
    :ModelBar2(recordLayerMbox)
    ,Rte::Db::Record::Base( m_nullPoint4Viewer,
                            *this,
                            myRecordList, 
                            delayWriteTimeInMsec, 
                            "RecordBar2",
                            recordLayerMbox, 
                            timingSource, 
                            eventLogger 
                          )
        {
        }


public: 
    /// See Rte::Point::Model::Api
    void defaultMe( void ) throw()
        {
        // Note: The model point starts out with all Tuples/elements being INVALID

        // Default to only 1 (of 4) Tuples being 'valid/existing'
        addTuple(2);
        m_tuples_[2].setAllValidState(RTE_ELEMENT_API_STATE_VALID);
        m_tuples_[2].m_flag.set( true );
        m_tuples_[2].m_text.set( "<name>" );
        m_tuples_[2].m_data.set( 23 );
        }

};




#endif 