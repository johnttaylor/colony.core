#ifndef Cpl_Rte_Persistence_Framing_h_
#define Cpl_Rte_Persistence_Framing_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2018  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */



///
namespace Cpl {
namespace Rte {
namespace Persistence {


/** Persistent storage Framing details
    <pre>

    Storage 'File'
    +----------+---------------+-----+---------+
    | Chunk 1* | Chunk 2       | ... | Chunk N |
    +----------+---------------+-----+---------+


    An Individual Chunk:

    Chunk Layer
    +-------+-------------------------------------------------------------------------------+-------+
    | clen  | cdata                                                                         | ccrc  |
    +-------+-------------------------------------------------------------------------------+-------+

            Record Layer
            +-------+-------+---------------------------------------------------------------+
            | nlen  | name  | rdata                                                         |
            +-------+-------+---------------------------------------------------------------+

                             Serialized Model Point (SMP) Layer
                            +---------+---------+---------+---------+---------+---------+---+
                            | M1-nlen | M1-name | M1-data | M2-nlen | M2-name | M2-data |...|
                            +---------+---------+---------+---------+---------+---------+---+


   *NOTE: The first Chunk is the file is reserved for storing
          the database signature string.  The signature string identifies
          individual ModelDatabaseApi instances as well as versioning its
          storage 'schema', i.e. if there is not an exact match of the stored
          signature string with the expected signature string it is considered
          a major/breaking change.

 </pre>
 */

 /** This enum defines the size of the framing fields for the database
  */
enum Framing_T
{
    eCLEN_SIZE=sizeof( uint32_t ),
    eCCRC_SIZE=sizeof( uint32_t ),
    eNLEN_SIZE=sizeof( uint16_t )
};


};      // end namespaces
};
};
#endif  // end header latch
