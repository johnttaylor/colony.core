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

#include "colony_config.h"
#include "Catch/catch.hpp"
#include "Cpl/Checksum/Crc32EthernetFast.h"
#include "Cpl/Itc/MailboxServer.h"
#include "Cpl/Itc/SyncReturnHandler.h"
#include "Cpl/Itc/CloseSync.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/File/Api.h"
#include "Cpl/Container/DList.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Rte/Db/Chunk/Server.h"
#include "Rte/Db/Chunk/FileSystem.h"
#include "Rte/Db/Record/Api.h"
#include "Rte/Db/Record/Handler.h"
#include "Rte/Db/Record/Core.h"


/// This method is used as part of 'forcing' this object to being actualled 
/// linked during the NQBP link process (it is artifact of linking libraries 
/// and how CATCH auto-registers (via static objects) test case
void link_record(void) {}

using namespace Rte::Db::Record;


////////////////////////////////////////////////////////////////////////////////
#define DB_FNAME_           "myDbFile"
#define SIGNATURE_          "12346789"

#define RECORD_BUF_SIZE     100


static Cpl::Checksum::Crc32EthernetFast  chunkCrc_;
static Rte::Db::Chunk::FileSystem        dbMedia( DB_FNAME_ );
static Rte::Db::Chunk::Server            chunkServer_( dbMedia, chunkCrc_, "12346789" );

static Cpl::Itc::MailboxServer           chunkMailbox_;
static Cpl::Itc::MailboxServer           upperLayersMailbox_;

static uint8_t                           buffer_[RECORD_BUF_SIZE];


#define SECT_     "_0test"





////////////////////////////////////////////////////////////////////////////////
class MySet: public Api
{
public:
    Rte::Db::Chunk::Handle  m_chunkHdl;
    const char*             m_name;
    uint8_t*                m_dataPtr;
    uint32_t                m_datalen;
    Handler&                m_writer;
    uint8_t                 m_defaultValue;

public:
    ///
    MySet( const char* name, uint8_t* dataPtr,  uint32_t dataLen, uint8_t defaultValue, Handler& writeApi )
        :m_name(name)
        ,m_dataPtr(dataPtr)
        ,m_datalen(dataLen)
        ,m_writer(writeApi)
        ,m_defaultValue(defaultValue)
            {
            }

public:
    ///
    const char* getName(void) const                     { return m_name; }
    ///
    void setChunkHandle( Rte::Db::Chunk::Handle& src )  { m_chunkHdl = src; }
    ///
    Rte::Db::Chunk::Handle& getChunkHandle(void)        { return m_chunkHdl; }

    ///
    void notifyRead( void* srcBuffer, uint32_t dataLen )
        {
        if ( dataLen >= m_datalen )
            {
            Cpl::System::FatalError::logf( "MySet::notifyRead - Buffer length Error." );
            }

        memcpy( m_dataPtr, srcBuffer, dataLen );
        }

    ///
    uint32_t fillWriteBuffer( void* dstBuffer, uint32_t maxDataSize )
        {
        CPL_SYSTEM_TRACE_MSG(SECT_, ("MySet::fillWriteBuffer"));
        if ( maxDataSize < m_datalen )
            {
            Cpl::System::FatalError::logf( "MySet::fillWriteBuffer - Buffer length Error." );
            }

        memcpy(dstBuffer, m_dataPtr, m_datalen);
        return m_datalen; 
        }

    ///
    void notifyWriteDone(void)
        {
        CPL_SYSTEM_TRACE_MSG(SECT_, ("MySet::notifyWriteDone"));
        }

public:
    ///
    void defaultSet(void) throw()
        {
        memset(m_dataPtr, m_defaultValue, m_datalen );
        }
};


/// 
class SetHandler: public Handler::Client,
                  public Cpl::Container::DList<MySet>,
                  public Cpl::Itc::CloseSync
{
public:
    int                                 m_count;
    Cpl::Itc::CloseRequest::CloseMsg*   m_pendingCloseMsgPtr;
    Core                                m_recordHandler;


public:
    ///
    SetHandler( uint8_t* buffer, uint32_t bufsize, Rte::Db::Chunk::Request::SAP& chunkSAP, Cpl::Itc::PostApi& upperLayersMbox, ErrorClient* errHandlerPtr=0 )
        :Cpl::Itc::CloseSync(upperLayersMbox)
        ,m_count(0)
        ,m_pendingCloseMsgPtr(0)
        ,m_recordHandler(buffer, bufsize, *this, chunkSAP, upperLayersMbox, Cpl::Log::Loggers::null(), errHandlerPtr )
            {
            }


public:
    /// 
    void registerSet( MySet& setToRegister )
        {
        put( setToRegister );
        }


public:
    ///
    Api* getRecordApi( const char* recName, uint16_t nameLen )
        {
        MySet* itemPtr = first();
        while( itemPtr )
            {
            if ( strncmp( recName, itemPtr->getName(), nameLen ) == 0 )
                {
                CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::getRecordApi, name=%s, found=%p, old count=%d", recName, itemPtr, m_count));
                m_count--;
                return itemPtr;
                }

            itemPtr = next( *itemPtr );
            }

        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::getRecordApi, NO MATCH FOUND FOR: name=%s, old count=%d", recName, m_count));
        return 0;
        }

    ///
    void notifyOpenCompleted(void)
        {
        issueWrites();
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::notifyOpenCompleted. count=%d", m_count));
        }

    ///
    void notifyOpenFailed(void) 
        {
        issueWrites();
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::notifyOpenFailed.count=%d", m_count));
        }

    ///
    void notifyIncompatible(void) 
        {
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::notifyIncompatible.count=%d", m_count));
        }

    ///
    bool isCleanLoad(void)
        {
        return m_count == 0;
        }

    ///
    void notifyStopped(void)
        {
        m_pendingCloseMsgPtr->returnToSender();
        }


public:
    ///
    void request( Cpl::Itc::OpenRequest::OpenMsg& msg )
        {
        m_count = countSets();;
        defaultSets();
        m_recordHandler.start();
        msg.returnToSender();
        }

    ///
    void request( Cpl::Itc::CloseRequest::CloseMsg& msg )
        {
        m_recordHandler.stop();
        m_pendingCloseMsgPtr = &msg;
        }


public:
    ///
    void issueWrites(void)
        {
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::issueWrites"));
        MySet* itemPtr = first();
        while( itemPtr )
            {
            m_recordHandler.write( *itemPtr );
            itemPtr = next(*itemPtr);
            }
        }

    ///
    void defaultSets(void)
        {
        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::defaultSets"));
        MySet* itemPtr = first();
        while( itemPtr )
            {
            itemPtr->defaultSet();
            itemPtr = next(*itemPtr);
            }
        }

    /// 
    int countSets(void)
        {
        int count = 0;
        MySet* itemPtr = first();
        while( itemPtr )
            {
            count++;
            itemPtr = next(*itemPtr);
            }

        CPL_SYSTEM_TRACE_MSG(SECT_, ("SetHandler::countSets. count=%d", count));
        return count;
        }
};




////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "record", "[record]" )
    {
    CPL_SYSTEM_TRACE_FUNC( SECT_ );
    Cpl::System::Shutdown_TS::clearAndUseCounter();

    // Create Threads
    Cpl::System::Thread* chunkThreadPtr       = Cpl::System::Thread::create( chunkMailbox_,        "CHUNK-LAYER" );
    Cpl::System::Thread* upperLayersThreadPtr = Cpl::System::Thread::create( upperLayersMailbox_,  "RECORD/SET-LAYERS" );


    // Delete the previous DB file (if it exists)
    Cpl::Io::File::Api::remove( DB_FNAME_ );

    // Create the Record + Set layers
    Rte::Db::Chunk::Request::SAP chunkSAP(chunkServer_, chunkMailbox_ );
    SetHandler                   client( buffer_, sizeof(buffer_), chunkSAP, upperLayersMailbox_ );    // Contains both the Record and Set Handlers

    // Start the Record/Set layers
    client.open();

    // Allow time for stuff to run
//    Cpl::System::Api::sleep( 100 );


    // Stop the Record/Set layers
    client.close();
    
    // Shutdown threads
    chunkMailbox_.pleaseStop();
    upperLayersMailbox_.pleaseStop();
    Cpl::System::Api::sleep(100);       // allow time for threads to stop
    REQUIRE( chunkThreadPtr->isRunning() == false );
    REQUIRE( upperLayersThreadPtr->isRunning() == false );
    Cpl::System::Thread::destroy( *chunkThreadPtr );
    Cpl::System::Thread::destroy( *upperLayersThreadPtr );
    REQUIRE( Cpl::System::Shutdown_TS::getAndClearCounter() == 0 );
    }
