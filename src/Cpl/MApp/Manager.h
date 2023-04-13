#ifndef Cpl_MApp_Manager_h_
#define Cpl_MApp_Manager_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Itc/CloseSync.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/MApp/ManagerApi.h"
#include "Cpl/MApp/Requests.h"

///
namespace Cpl {
///
namespace MApp {


/** This concrete class implements the MApp Manager.  The MApp Manager is 
    the 'public facing' entity that is responsible for starting, stopping, etc.
    MApp instances.  
 */
class Manager: 
    public ManagerApi, 
    public Cpl::Itc::CloseSync,
    public StartMAppRequest,
    public StopMAppRequest,
    public GetAvailableMAppRequest,
    public GetStartedMAppRequest,
    public LookupMAppRequest
{
public:
    /// Constructor
    Manager( Cpl::Dm::MailboxServer&     myMbox, 
             Cpl::Container::SList<Api>& listOfMApps );

    /// Destructor
    ~Manager() {}

public:
    /// Perform the in-thread initialization of the service
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg );

    /// Performs the in-thread shutdown of the service
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg );


public:
    /// See Cpl::MApp::Api
    bool startMApp( const char* name, const char* args ) noexcept;

    /// See Cpl::MApp::Api
    void stopMApp( const char* name ) noexcept;

    /// See Cpl::MApp::Api
    bool getAvailableMApps( Cpl::MApp::Api* dstList[], size_t dstMaxElements, size_t& numElemsFound ) noexcept;

    /// See Cpl::MApp::Api
    bool getStartedMApps( Cpl::MApp::Api* dstList[], size_t dstMaxElements, size_t& numElemsFound ) noexcept;

    /// See Cpl::MApp::Api
    Cpl::MApp::Api* lookUpMApp( const char* mappName ) noexcept;


public:
    /// Set Cpl::MApp::Requests
    void request( StartMAppMsg& msg );

    /// Set Cpl::MApp::Requests
    void request( StopMAppMsg& msg );

    /// Set Cpl::MApp::Requests
    void request( GetAvailableMAppMsg& msg );

    /// Set Cpl::MApp::Requests
    void request( GetStartedMAppMsg& msg );

    /// Set Cpl::MApp::Requests
    void request( LookupMAppMsg& msg );

protected:
    /// List of inactive MApps
    Cpl::Container::SList<Api>& m_inactiveMApps;

    /// List of started MApps
    Cpl::Container::SList<Api>  m_startedMApps;

    /// My open/close state
    bool                        m_opened;
};

};      // end namespaces
};
#endif  // end header latch