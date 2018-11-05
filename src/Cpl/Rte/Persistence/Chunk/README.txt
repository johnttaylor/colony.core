/** @file */
/** @namespace Cpl::Rte::Persistence::Chunk
@htmlonly
<pre>    

The 'Chunk' namespace contains the interfaces, classes, infrastructure, etc.
for performing the 'file IO' for the RTE persistent storage.  The Chunk layer
is responsible for ensuring data integrity (aka CRC checking) of all
records stored.  The 'file IO' that the Chunk layer uses is abstracted so that 
the Chunk layer can work with persistent storage media that is not a 
traditional file system.

</pre>    
@endhtmlonly
*/  


  