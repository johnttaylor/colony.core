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

// Magic constants for testing
#define REC_BAR1_TUP1_DEFAULT_T1            "1"
#define REC_BAR1_TUP1_DEFAULT_T3            "<3>"
#define REC_BAR1_TUP1_DEFAULT_T8            "n/a"
#define REC_BAR1_TUP2_DEFAULT_T10           "<my label>"
#define REC_BAR1_TUP1_DEFAULT_VALID_T1      true
#define REC_BAR1_TUP1_DEFAULT_VALID_T3      true
#define REC_BAR1_TUP1_DEFAULT_VALID_T8      false
#define REC_BAR1_TUP2_DEFAULT_VALID_T10     true

#define REC_BAR2_TUP0_DEFAULT_INCONTAINER   false
#define REC_BAR2_TUP0_DEFAULT_T3            "n/a"
#define REC_BAR2_TUP0_DEFAULT_T5            "n/a"
#define REC_BAR2_TUP0_DEFAULT_VALID_T3      false
#define REC_BAR2_TUP0_DEFAULT_VALID_T5      false

#define REC_BAR2_TUP1_DEFAULT_INCONTAINER   false
#define REC_BAR2_TUP1_DEFAULT_T3            "n/a"
#define REC_BAR2_TUP1_DEFAULT_T5            "n/a"
#define REC_BAR2_TUP1_DEFAULT_VALID_T3      false
#define REC_BAR2_TUP1_DEFAULT_VALID_T5      false

#define REC_BAR2_TUP2_DEFAULT_INCONTAINER   true
#define REC_BAR2_TUP2_DEFAULT_T3            "333"
#define REC_BAR2_TUP2_DEFAULT_T5            "<555>"
#define REC_BAR2_TUP2_DEFAULT_VALID_T3      true
#define REC_BAR2_TUP2_DEFAULT_VALID_T5      true

#define REC_BAR2_TUP3_DEFAULT_INCONTAINER   false
#define REC_BAR2_TUP3_DEFAULT_T3            "n/a"
#define REC_BAR2_TUP3_DEFAULT_T5            "n/a"
#define REC_BAR2_TUP3_DEFAULT_VALID_T3      false
#define REC_BAR2_TUP3_DEFAULT_VALID_T5      false

#define REC_BAR2EXT_TUP0_DEFAULT_INCONTAINER   true
#define REC_BAR2EXT_TUP0_DEFAULT_T3            "111"
#define REC_BAR2EXT_TUP0_DEFAULT_T5            "*5*5*"
#define REC_BAR2EXT_TUP0_DEFAULT_VALID_T3      true
#define REC_BAR2EXT_TUP0_DEFAULT_VALID_T5      true

#define REC_BAR2EXT_TUP1_DEFAULT_INCONTAINER   false
#define REC_BAR2EXT_TUP1_DEFAULT_T3            "n/a"
#define REC_BAR2EXT_TUP1_DEFAULT_T5            "n/a"
#define REC_BAR2EXT_TUP1_DEFAULT_VALID_T3      false
#define REC_BAR2EXT_TUP1_DEFAULT_VALID_T5      false

#define REC_BAR2EXT_TUP2_DEFAULT_INCONTAINER   false
#define REC_BAR2EXT_TUP2_DEFAULT_T3            "n/a"
#define REC_BAR2EXT_TUP2_DEFAULT_T5            "n/a"
#define REC_BAR2EXT_TUP2_DEFAULT_VALID_T3      false
#define REC_BAR2EXT_TUP2_DEFAULT_VALID_T5      false

#define REC_BAR2EXT_TUP3_DEFAULT_INCONTAINER   false
#define REC_BAR2EXT_TUP3_DEFAULT_T3            "n/a"
#define REC_BAR2EXT_TUP3_DEFAULT_T5            "n/a"
#define REC_BAR2EXT_TUP3_DEFAULT_VALID_T3      false
#define REC_BAR2EXT_TUP3_DEFAULT_VALID_T5      false

#define REC_BAR2EXT_TUP4_DEFAULT_INCONTAINER   false
#define REC_BAR2EXT_TUP4_DEFAULT_T3            "n/a"
#define REC_BAR2EXT_TUP4_DEFAULT_T5            "n/a"
#define REC_BAR2EXT_TUP4_DEFAULT_VALID_T3      false
#define REC_BAR2EXT_TUP4_DEFAULT_VALID_T5      false

#define REC_BAR2EXT_TUP5_DEFAULT_INCONTAINER   true
#define REC_BAR2EXT_TUP5_DEFAULT_T3            "66."
#define REC_BAR2EXT_TUP5_DEFAULT_T5            "5a5a5"
#define REC_BAR2EXT_TUP5_DEFAULT_VALID_T3      true
#define REC_BAR2EXT_TUP5_DEFAULT_VALID_T5      true



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
        m_fields1.m_text1.set( REC_BAR1_TUP1_DEFAULT_T1 );
        m_fields1.m_text3.set( REC_BAR1_TUP1_DEFAULT_T3 );
        m_fields1.m_text8.setInvalid();  // Example of optional Element
    
        // Default Tuple2
        m_fields2.m_text10.set( REC_BAR1_TUP2_DEFAULT_T10 );
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
        m_tuples_[2].m_text3.set( REC_BAR2_TUP2_DEFAULT_T3 );
        m_tuples_[2].m_text5.set( REC_BAR2_TUP2_DEFAULT_T5 );
        }

};

/** Concrete Record: BAR2Ext
 */
class RecordBar2Ext: public ModelBar2Ext,
                     public Rte::Db::Record::Base

{
public:
    ///
    PointBar2ExtNull   m_nullPoint4Viewer;

public:
    /// Constructor
    RecordBar2Ext( Cpl::Container::Map<ApiLocal>& myRecordList,
                   Cpl::Itc::PostApi&             recordLayerMbox, 
                   Cpl::Timer::CounterSource&     timingSource,
                   unsigned long                  delayWriteTimeInMsec = 0,    // Default is: no delay
                   Cpl::Log::Api&                 eventLogger = Cpl::Log::Loggers::application()
                 )
    :ModelBar2Ext(recordLayerMbox)
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

        // Default to only 2 (of 6) Tuples being 'valid/existing'
        addTuple(0);
        m_tuples_[0].setAllValidState(RTE_ELEMENT_API_STATE_VALID);
        m_tuples_[0].m_text3.set( REC_BAR2EXT_TUP0_DEFAULT_T3 );
        m_tuples_[0].m_text5.set( REC_BAR2EXT_TUP0_DEFAULT_T5 );
        addTuple(5);
        m_tuples_[5].setAllValidState(RTE_ELEMENT_API_STATE_VALID);
        m_tuples_[5].m_text3.set( REC_BAR2EXT_TUP5_DEFAULT_T3 );
        m_tuples_[5].m_text5.set( REC_BAR2EXT_TUP5_DEFAULT_T5 );
        }

};




#endif 