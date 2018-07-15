#ifndef Rte_Db_Framing_h_
#define Rte_Db_Framing_h_
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

#include <stdint.h>


///
namespace Rte { namespace Db { 


/** Database Framing details
    <pre>

    Database
    +----------+---------------+-----+---------+
    | Chunk 1* | Chunk 2       | ... | Chunk N |
    +----------+---------------+-----+---------+


    An Individual Chunk: 
    
    Chunk Layer
    +-------+-----------------------------------------------+-------+
    | clen  | cdata                                         | ccrc  |  
    +-------+-----------------------------------------------+-------+

            Record Layer
            +-------+-------+-------------------------------+
            | nlen  | name  | rdata                         |
            +-------+-------+-------------------------------+

                            Set Layer
                            +----+----+----------------+----+
                            | P1 | P2 | ...            | Pn |
                            +----+----+----------------+----+


   *NOTE: The first Chunk is the database is reserved for storing
          the database signature string.  The signature string idendifies
          individual databases as well as versioning their 'scheme'

 </pre>
 */

/** This enum defines the size of the framing fields for the database
 */
enum Framing_T  { eCLEN_SIZE=sizeof(uint32_t),
                  eCCRC_SIZE=sizeof(uint32_t),
                  eNLEN_SIZE=sizeof(uint16_t)
                };


};      // end namespaces
};
#endif  // end header latch
