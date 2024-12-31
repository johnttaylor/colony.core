/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "master_eeprom.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"

#define SECT_	"_0test"

#define STOP_TEST()         for ( ;; )
using namespace Driver::SPI;

static    uint8_t srcBuffer_[10];
static    uint8_t dstBuffer_[10];

void runtests( Driver::SPI::Master& uut, Driver::DIO::Out& cs )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("SPI Driver Test - expecting a CAT24C512 (or compatible) EEPROM peripheral device.") );

    // Initialize ChipSelect
    bool result = cs.start( false );
    if ( !result )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("FAILED to start the CS driver") );
        STOP_TEST();
    }

    // Verify not-started is bad thing
    srcBuffer_[0] = 5;  // Read Status Reg Command
    cs.assertOutput();
    result = uut.transfer( 3, srcBuffer_, dstBuffer_ );
    cs.deassertOutput();
    if ( !result )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED: driver not started") );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] unexpected result code") );
        STOP_TEST();
    }

    // Start the driver
    bool started = uut.start();
    if ( !started )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR: uut.start() failed" ) );
        STOP_TEST();
    }

    // Read the Status register to see if it connected
    srcBuffer_[0] = 5;  // Read Status Reg Command
    memset( dstBuffer_, 0xA5, sizeof( dstBuffer_ ) );
    cs.assertOutput();
    result = uut.transfer( 3, srcBuffer_, dstBuffer_ );
    cs.deassertOutput();

    if ( result && dstBuffer_[1] == 0x00 && dstBuffer_[2] == 0x00 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED: Slave device is present" ) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR: Slave device did not respond (%02x %02x)", dstBuffer_[1], dstBuffer_[2] )  );
        STOP_TEST();
    }

    // Write Enable (0x06)
    srcBuffer_[0] = 6;
    cs.assertOutput();
    result = uut.transfer( 1, srcBuffer_ );
    cs.deassertOutput();
    if ( result  )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED: Write Enable") );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR: Write Enable failed") );
        STOP_TEST();
    }

    // Write 3 bytes to the eeprom
    srcBuffer_[0] = 2;  // Write command
    srcBuffer_[1] = 0;  // Start writing at offset zero (24 bit address)
    srcBuffer_[2] = 0;  
    srcBuffer_[3] = 0;  
    srcBuffer_[4] = 1;
    srcBuffer_[5] = 2;
    srcBuffer_[6] = 3;
    cs.assertOutput();
    result = uut.transfer( 7, srcBuffer_ );
    cs.deassertOutput();
    if ( result  )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED: Wrote 3 bytes to offset 0") );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR: Failed to write 3 bytes at offset 0") );
        STOP_TEST();
    }
    
    // Cheat and do brute force wait to allow the write to complete
    Cpl::System::Api::sleep( 100 );

    // Read back the bytes
    memset( srcBuffer_, 0, sizeof( srcBuffer_ ) );
    srcBuffer_[0] = 3;  // Read command
    srcBuffer_[1] = 0;  // Start reading at offset zero (24 bit address)
    srcBuffer_[2] = 0;
    srcBuffer_[3] = 0;
    cs.assertOutput();
    result = uut.transfer( 10, srcBuffer_, dstBuffer_ );
    cs.deassertOutput();
    if ( result && dstBuffer_[4] == 1 && dstBuffer_[5] == 2 && dstBuffer_[6] == 3 )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED: Read 3 bytes to offset 0.") );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR: Failed to read 3 bytes at offset 0 (act=%02x %02x %02x | ex=01 02 03", dstBuffer_[4], dstBuffer_[5], dstBuffer_[6]) );
        STOP_TEST();
    }

#if 0
    // Wait for write to complete (add it resets the read-pointer)
    bool writeCompleted = false;
    for ( int i=0; i < 10; i++ )
    {
        buffer[0] = 0;  // Start writing at offset zero
        buffer[1] = 0;
        result = uut.writeToDevice( device7BitAddress, 2, buffer );
        if ( result == Driver::I2C::Master::eSUCCESS )
        {
            writeCompleted = true;
            break;
        }
        Cpl::System::Api::sleep( 100 );
    }
    if ( !writeCompleted )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] EEPROM not responding after write", device7BitAddress, result) );
        STOP_TEST();
    }

    // Read back the bytes
    result = uut.readFromDevice( device7BitAddress, 3, buffer );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED:[%x] Read 3 bytes to offset 0. %02x %02x %02x | 02 03 04", device7BitAddress, buffer[0], buffer[1], buffer[2]) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] Failed to read 3 bytes at offset 0: %d", device7BitAddress, result) );
        STOP_TEST();
    }

    // Write some more bytes to eeprom
    buffer[0] = 0;  // Start writing at offset zero
    buffer[1] = 0;
    buffer[2] = 22;
    buffer[3] = 33;
    buffer[4] = 44;
    result = uut.writeToDevice( device7BitAddress, 2 + 3, buffer );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED:[%x] Wrote 3 more bytes to offset 0", device7BitAddress) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] Failed to write 3 more bytes at offset 0: %d", device7BitAddress, result) );
        STOP_TEST();
    }

    // Wait for write to complete (add it resets the read-pointer)
    writeCompleted = false;
    for ( int i=0; i < 10; i++ )
    {
        buffer[0] = 0;  // Start writing at offset zero
        buffer[1] = 0;
        result = uut.writeToDevice( device7BitAddress, 2, buffer );
        if ( result == Driver::I2C::Master::eSUCCESS )
        {
            writeCompleted = true;
            break;
        }
        Cpl::System::Api::sleep( 100 );
    }
    if ( !writeCompleted )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] EEPROM not responding after write", device7BitAddress, result) );
        STOP_TEST();
    }
    
    // Read back the bytes
    result = uut.readFromDevice( device7BitAddress, 3, buffer );
    if ( result == Driver::I2C::Master::eSUCCESS )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("EXPECTED:[%x] Read 3 more bytes to offset 0. %02x %02x %02x | 16 21 2c", device7BitAddress, buffer[0], buffer[1], buffer[2]) );
    }
    else
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR:[%x] Failed to read 3 more bytes at offset 0: %d", device7BitAddress, result) );
        STOP_TEST();
    }
#endif

    // Do not return once the test has completed
    CPL_SYSTEM_TRACE_MSG( SECT_, ("ALL TESTS COMPLETED SUCCESSFULLY") );
    STOP_TEST();
}
