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

#include "littlefs/lfs.h"

// variables used by the filesystem
static lfs_t      lfs;
static lfs_file_t file;

void runtest( const lfs_config& cfg )
{
    // mount the filesystem
    int err = lfs_mount( &lfs, &cfg );

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if ( err )
    {
        lfs_format( &lfs, &cfg );
        lfs_mount( &lfs, &cfg );
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open( &lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT );
    lfs_file_read( &lfs, &file, &boot_count, sizeof( boot_count ) );

    // update boot count
    boot_count += 1;
    lfs_file_rewind( &lfs, &file );
    lfs_file_write( &lfs, &file, &boot_count, sizeof( boot_count ) );

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close( &lfs, &file );

    // release any resources we were using
    lfs_unmount( &lfs );

    // print the boot count
    printf( "boot_count: %d\n", boot_count );
}
