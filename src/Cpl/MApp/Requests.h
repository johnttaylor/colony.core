#ifndef Cpl_MApp_Requests_h_
#define Cpl_MApp_Requests_h_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Cpl/Itc/RequestMessage.h"
#include "Cpl/Itc/ResponseMessage.h"
#include "Cpl/Itc/SAP.h"
#include "Cpl/MApp/Api.h"

///
namespace Cpl {
///
namespace MApp {


/** This abstract class define ITC message type and payload for the application
    to start a MApp
 */
class StartMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<StartMAppRequest> SAP;

public:
    /// Payload for Message: GetLaMApp
    class Payload
    {
    public:
        /// INPUT: The name of the MApp to run
        const char* m_mappName;

        /** OUTPUT: results
            true  = MApp was found and started
            false = no such MApp
         */
        bool        m_success;

    public:
        /// Constructor. Use for getLaMApp() message
        Payload( const char* mappName )
            :m_mappName( mappName ), m_success( false )
        {
        }
    };


public:
    /// Message Type: Start
    typedef Cpl::Itc::RequestMessage<StartMAppRequest, Payload> StartMAppMsg;

    /// Request: Start message
    virtual void request( StartMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~StartMAppRequest() {}
};



////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to stop the current MApp

 */
class StopMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<StopMAppRequest> SAP;

public:
    /// Payload for Message: StopMApp (No actual Data -->just a type name)
    class Payload
    {
    public:
        /// INPUT: The name of the MApp to stop
        const char* m_mappName;

        /** OUTPUT: results
            true  = the MApp was stopped
            false = the specified MApp was not the started state
         */
        bool        m_success;
    public:
        /// Constructor. Use for getLaMApp() message
        Payload( const char* mappName )
            :m_mappName( mappName ), m_success( false )
        {
        }
    };

public:
    /// Message Type: Stop
    typedef Cpl::Itc::RequestMessage<StopMAppRequest, Payload> StopMAppMsg;

    /// Request: Stop message
    virtual void request( StopMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~StopMAppRequest() {}
};


////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to pause the current MApp

 */
class PauseMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<PauseMAppRequest> SAP;

public:
    /// Payload for Message: PauseMApp (No actual Data -->just a type name)
    class Payload
    {
    public:
        /// Constructor. 
        Payload() {}
    };

public:
    /// Message Type: Pause
    typedef Cpl::Itc::RequestMessage<PauseMAppRequest, Payload> PauseMAppMsg;

    /// Request: Pause message
    virtual void request( PauseMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~PauseMAppRequest() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to resume the current MApp

 */
class ResumeMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<ResumeMAppRequest> SAP;

public:
    /// Payload for Message: ResumeMApp (No actual Data -->just a type name)
    class Payload
    {
    public:
        /// Constructor. 
        Payload() {}
    };

public:
    /// Message Type: Pause
    typedef Cpl::Itc::RequestMessage<ResumeMAppRequest, Payload> ResumeMAppMsg;

    /// Request: Pause message
    virtual void request( ResumeMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~ResumeMAppRequest() {}
};

////////////////////////////////////////////////////////////////////////////////
/** This abstract class define ITC message type and payload for the application
    to resume the current MApp

 */
class GetStartedMAppRequest
{
public:
    /// SAP for this API
    typedef Cpl::Itc::SAP<GetStartedMAppRequest> SAP;

public:
    /// Payload for Message: 
    class Payload
    {
    public:
        /// Memory to store the returned list
        Cpl::MApp::Api** dstList;

        /// Maximum number of elements that can be stored in 'dstList'
        size_t           dstMaxElements;

    public:
        /// Constructor. 
        Payload() {}
    };

public:
    /// Message Type: Pause
    typedef Cpl::Itc::RequestMessage<GetStartedMAppRequest, Payload> GetStartedMAppMsg;

    /// Request: Pause message
    virtual void request( GetStartedMAppMsg& msg ) = 0;

public:
    /// Virtual Destructor
    virtual ~GetStartedMAppRequest() {}
};


};      // end namespaces
};
#endif  // end header latch
