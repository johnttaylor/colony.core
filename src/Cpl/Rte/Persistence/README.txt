/** @file */
/** @namespace Cpl::Rte::Persistence
@htmlonly
<pre>    

The 'Persistence' namespace provides a basic persistence storage mechanism 
for the RTE's Model Points.  The persistence sub-system has the following 
features:

    o The sub-system organizes persistent Model Points into Records.  The 
      Application is responsible for defining which Model Points are in which 
      Record. A Record has reference(s) to one or more Model Points.  When any 
      of these Model Points are change - the entire Record is updated to 
      persistent storage, i.e. the 'Record' is the unit of atomic read/write 
      operations from/to persistent storage.

    o On start-up the Records are read and the retrieved values are written
      to the Model Points associated with each Record.

    o The sub-system monitors all Model Points with all Records for changes. On 
      change the persistent storage media is updated with the new value(s).

    o All persistently stored data/meta-data is CRC'd to detect data corruption. 
      Note: The CRCs are only validated on start-up. 
    
    o The sub-system provides a mechanism for defaulting Records on the
      initial run of the Application and/or when a Record is determined to be 
      corrupt (i.e. bad CRC when reading from persistent storage).

    o The sub-system supports adding new Records or adding new Model Points to 
      an existing Record without a change to its schema, i.e. automatically 
      upgrade an existing persistent 'file' when 'additive' changes are made to 
      the data being persistently stored.

    o The sub-system supports deleting Records or deleting Model Points from 
      an existing Record without a change to its schema, i.e. automatically 
      upgrade an existing persistent 'file' when 'depreciative' changes are 
      made to the data being persistently stored.   Note: the 'deletions' must
      truly be depreciative, i.e. NO CONVERSION OF ANY KIND will happen when
      a Record/Model Point is deleted.

    o The sub-system independent of the physical persistent storage
      media (i.e. all IO is done through the Cpl::Io interfaces).

    o Each Cpl::Rte::DatabaseApi instance has its own persistent 'file' and
      it owns persistent storage 'server' instance.


</pre>    
@endhtmlonly
*/  


  