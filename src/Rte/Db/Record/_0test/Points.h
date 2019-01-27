#ifndef Points_h_
#define Points_h_
/*----------------------------------------------------------------------------- 
* This file is part of the Colony.Core Project.  The Colony.Core Project is an   
* open source project with a BSD type of licensing agreement.  See the license  
* agreement (license.txt) in the top/ directory or on the Internet at           
* http://integerfox.com/colony.core/license.txt
*                                                                               
* Copyright (c) 2014-2019  John T. Taylor                                        
*                                                                               
* Redistributions of the source code must retain the above copyright notice.    
*----------------------------------------------------------------------------*/ 
/** @file */

#include "Rte/Point/Basic.h"
#include "Rte/Point/Container.h"
#include "Rte/Point/Model/Base.h"
#include "Rte/Point/Controller/Base.h"
#include "Rte/Point/Controller/Container.h"
#include "Rte/Point/Query/Base.h"
#include "Rte/Point/Query/Container.h"
#include "Rte/Point/Viewer/Composer.h"
#include "Rte/Point/Viewer/Container.h"
#include "Rte/Tuple/Null.h"
#include "Rte/Tuple/NullItem.h"
#include "Tuples.h"


/*------------------------- POINTS ------------------------------------------*/
/** Concrete Point: BAR1
 */
class PointBar1: public Rte::Point::Basic<2>
{
public:
    ///
    TupleFoo1   m_fields1;
    ///
    TupleFoo3   m_fields2;

public:
    /// Constructor
    PointBar1( void )
        {
        registerTuple( 0, m_fields1 );
        registerTuple( 1, m_fields2 );
        }

};

/** Concrete Point; Null Point for BAR1
 */
class PointBar1Null: public Rte::Point::Basic<2>
{
public:
    ///
    Rte::Tuple::Null m_fields1;
    ///
    Rte::Tuple::Null m_fields2;

public:
    /// Constructor
    PointBar1Null( void )
        {
        registerTuple( 0, m_fields1 );
        registerTuple( 1, m_fields2 );
        }

};


/** Concrete Point: BAR2 (Container Point that hold 4 instances of the Foo2 tuple)
 */
class PointBar2: public Rte::Point::Container<TupleFoo2,4>
{
public:
    /// Constructor
    PointBar2( void ){}

};

/** Concrete Point: Null Point for BAR2 (Container Point that hold 4 instances of the Foo2 tuple)
 */
class PointBar2Null: public Rte::Point::Container<Rte::Tuple::NullItem,4>
{
public:
    /// Constructor
    PointBar2Null( void ){}

};


/** Concrete Point: BAR2Ext (Container Point that hold 6 instances of the Foo2 tuple)
 */
class PointBar2Ext: public Rte::Point::Container<TupleFoo2,6>
{
public:
    /// Constructor
    PointBar2Ext( void ){}

};

/** Concrete Point: Null Point for BAR2Ext (Container Point that hold 6 instances of the Foo2 tuple)
 */
class PointBar2ExtNull: public Rte::Point::Container<Rte::Tuple::NullItem,6>
{
public:
    /// Constructor
    PointBar2ExtNull( void ){}

};



/*------------------------- MODEL POINTS ------------------------------------*/
/** Concrete Model Point: BAR1
 */
class ModelBar1: public PointBar1,
                 public Rte::Point::Model::Base
{
public:
    /// Constructor
    ModelBar1( Cpl::Itc::PostApi& myMbox )
        :Rte::Point::Model::Base(*this, myMbox)
            {
            }
};


/** Concrete Model Point: BAR2
 */
class ModelBar2: public PointBar2,
                 public Rte::Point::Model::Base
{
public:
    /// Constructor
    ModelBar2( Cpl::Itc::PostApi& myMbox )
        :Rte::Point::Model::Base(*this,myMbox)
            {
            }
};


/** Concrete Model Point: BAR2Ext
 */
class ModelBar2Ext: public PointBar2Ext,
                    public Rte::Point::Model::Base
{
public:
    /// Constructor
    ModelBar2Ext( Cpl::Itc::PostApi& myMbox )
        :Rte::Point::Model::Base(*this,myMbox)
            {
            }
};


/*------------------------- CONTROLLER POINTS -------------------------------*/
/** Concrete Controller Point: BAR1
 */
class ControllerBar1: public PointBar1,
                      public Rte::Point::Controller::Base
{
public:
    /// Constructor
    ControllerBar1( ModelBar1& modelPoint )
        :Rte::Point::Controller::Base(*this, modelPoint)
            {
            }

};


/** Concrete Controller Point: BAR2
 */
class ControllerBar2: public PointBar2,
                      public Rte::Point::Controller::Container
{
public:
    /// Constructor
    ControllerBar2( ModelBar2& modelPoint )
        :Rte::Point::Controller::Container(*this, modelPoint)
            {
            }

};


/** Concrete Controller Point: BAR2Ext
 */
class ControllerBar2Ext: public PointBar2Ext,
                         public Rte::Point::Controller::Container
{
public:
    /// Constructor
    ControllerBar2Ext( ModelBar2Ext& modelPoint )
        :Rte::Point::Controller::Container(*this, modelPoint)
            {
            }

};

/*------------------------- QUERY POINTS ------------------------------------*/
/** Concrete Query Point: BAR1
 */
class QueryBar1: public PointBar1,
                 public Rte::Point::Query::Base
{
public:
    /// Constructor
    QueryBar1( ModelBar1& modelPoint, Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY )
        :Rte::Point::Query::Base(*this, modelPoint, copyOption)
            {
            // Default to querying EVERYTHING
            setAllInUseState(true);
            }

};


/** Concrete Query Point: BAR2
 */
class QueryBar2: public PointBar2,
                 public Rte::Point::Query::Container
{
public:
    /// Constructor
    QueryBar2( ModelBar2& modelPoint, Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY )
        :Rte::Point::Query::Container(*this, modelPoint, copyOption)
            {
            // Default to querying EVERYTHING
            setAllInUseState(true);
            }

};


/** Concrete Query Point: BAR2Ext
 */
class QueryBar2Ext: public PointBar2Ext,
                    public Rte::Point::Query::Container
{
public:
    /// Constructor
    QueryBar2Ext( ModelBar2Ext& modelPoint, Rte::Point::Model::QueryRequest::Option_T copyOption = Rte::Point::Model::QueryRequest::eCOPY )
        :Rte::Point::Query::Container(*this, modelPoint, copyOption)
            {
            // Default to querying EVERYTHING
            setAllInUseState(true);
            }

};


/*------------------------- VIEWER POINTS -----------------------------------*/
/** Concrete Viewer Point: BAR1
 */
template <class CONTEXT>
class ViewerBar1: public PointBar1,
                  public Rte::Point::Viewer::Composer<CONTEXT>
{
public:
    /// Constructor
    ViewerBar1( CONTEXT&                                                                    context,
                typename Rte::Point::Viewer::Composer<CONTEXT>::ChangeNotificationFunc_T    contextChangedCb,
                typename Rte::Point::Viewer::Composer<CONTEXT>::StoppedNotificationFunc_T   contextStoppedCb,
                ModelBar1&                                                                  modelPoint,
                Cpl::Itc::PostApi&                                                          viewerMbox 
              )
    :Rte::Point::Viewer::Composer<CONTEXT>::Composer(*this, context, contextChangedCb, contextStoppedCb, modelPoint, viewerMbox)
        {}
};


/** Concrete LIGHT WEIGHT Viewer Point: BAR1
 */
template <class CONTEXT>
class LiteViewerBar1: public PointBar1Null,
                      public Rte::Point::Viewer::Composer<CONTEXT>
{
public:
    /// Constructor
    LiteViewerBar1( CONTEXT&                                                                    context,
                    typename Rte::Point::Viewer::Composer<CONTEXT>::ChangeNotificationFunc_T    contextChangedCb,
                    typename Rte::Point::Viewer::Composer<CONTEXT>::StoppedNotificationFunc_T   contextStoppedCb,
                    ModelBar1&                                                                  modelPoint,
                    Cpl::Itc::PostApi&                                                          viewerMbox 
                  )
    :Rte::Point::Viewer::Composer<CONTEXT>::Composer(*this, context, contextChangedCb, contextStoppedCb, modelPoint, viewerMbox)
        {}
};


/** Concrete Viewer Point: BAR2
 */
template <class CONTEXT>
class ViewerBar2: public PointBar2,
                  public Rte::Point::Viewer::Container<CONTEXT>
{
public:
    /// Constructor
    ViewerBar2( CONTEXT&                                                                    context,
                typename Rte::Point::Viewer::Container<CONTEXT>::ChangeNotificationFunc_T   contextChangedCb,
                typename Rte::Point::Viewer::Container<CONTEXT>::StoppedNotificationFunc_T  contextStoppedCb,
                ModelBar2&                                                                  modelPoint,
                Cpl::Itc::PostApi&                                                          viewerMbox 
              )
    :Rte::Point::Viewer::Container<CONTEXT>::Container(*this, context, contextChangedCb, contextStoppedCb, modelPoint, viewerMbox)
        {}
};


/** Concrete LIGHT WEIGHT Viewer Point: BAR2
 */
template <class CONTEXT>
class LiteViewerBar2: public PointBar2Null,
                      public Rte::Point::Viewer::Container<CONTEXT>
{
public:
    /// Constructor
    LiteViewerBar2( CONTEXT&                                                                    context,
                    typename Rte::Point::Viewer::Container<CONTEXT>::ChangeNotificationFunc_T   contextChangedCb,
                    typename Rte::Point::Viewer::Container<CONTEXT>::StoppedNotificationFunc_T  contextStoppedCb,
                    ModelBar2&                                                                  modelPoint,
                    Cpl::Itc::PostApi&                                                          viewerMbox 
                  )
    :Rte::Point::Viewer::Container<CONTEXT>::Container(*this, context, contextChangedCb, contextStoppedCb, modelPoint, viewerMbox)
        {}
};


/** Concrete Viewer Point: BAR2Ext
 */
template <class CONTEXT>
class ViewerBar2Ext: public PointBar2Ext,
                     public Rte::Point::Viewer::Container<CONTEXT>
{
public:
    /// Constructor
    ViewerBar2Ext( CONTEXT&                                                                    context,
                   typename Rte::Point::Viewer::Container<CONTEXT>::ChangeNotificationFunc_T   contextChangedCb,
                   typename Rte::Point::Viewer::Container<CONTEXT>::StoppedNotificationFunc_T  contextStoppedCb,
                   ModelBar2Ext&                                                               modelPoint,
                   Cpl::Itc::PostApi&                                                          viewerMbox 
                 )
    :Rte::Point::Viewer::Container<CONTEXT>::Container(*this, context, contextChangedCb, contextStoppedCb, modelPoint, viewerMbox)
        {}
};


/** Concrete LIGHT WEIGHT Viewer Point: BAR2Ext
 */
template <class CONTEXT>
class LiteViewerBar2Ext: public PointBar2ExtNull,
                         public Rte::Point::Viewer::Container<CONTEXT>
{
public:
    /// Constructor
    LiteViewerBar2Ext( CONTEXT&                                                                    context,
                       typename Rte::Point::Viewer::Container<CONTEXT>::ChangeNotificationFunc_T   contextChangedCb,
                       typename Rte::Point::Viewer::Container<CONTEXT>::StoppedNotificationFunc_T  contextStoppedCb,
                       ModelBar2Ext&                                                               modelPoint,
                       Cpl::Itc::PostApi&                                                          viewerMbox 
                     )
    :Rte::Point::Viewer::Container<CONTEXT>::Container(*this, context, contextChangedCb, contextStoppedCb, modelPoint, viewerMbox)
        {}
};

#endif 