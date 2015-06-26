/** @file */
/** @namespace Rte::Db
@htmlonly
<pre>    

The 'Db' (or database) namespace provides a basic persistance storage mechansim 
for the RTE's Model Points.  The database namespace is NOT a relational DB 
(et al.) or intended for runtime queries/joins/searches/etc. - its primary goal 
is to provide a backing persisetance storage to non-volitile media. The database 
has the following features:

    o The database organizes persistent Model Points into Sets.  The Application
      is responsible for defining which Model Points are in which db Sets. A Set 
      has reference(s) to one or persistent Model Points.  When any of these 
      Model Points are change - the entire Set is updated to persistent storage, 
      i.e. the 'Set' is the unit of atomic read/write operations from/to
      persistent storage.

    o On start-up the database is read and its retrieved values are written
      the designated Model Points.

    o The database monitors all Model Points with all Sets for changes.  On 
      change the persistent storage media is updated with the new value(s).

    o All database data is CRC'd to detect data corruption. Note: The CRCs are
      only validated on start-up. 
    
    o The database engine provides a mechansim for defaulting Sets on the
      initial run of the Application and/or when a Set is determined to be 
      corrupt (i.e. bad CRC when loading the database).

    o The database engine supports adding new Sets or adding new Model Points to 
      an existing Set without a change to its schema, i.e. automatically upgrade
      an existing db file when 'addative' changes are made to the data being
      persistenly stored.

    o The database engine is independent of the physical peristent storage
      media (i.e. all IO is done through the Cpl::Io interfaces).


</pre>    
@endhtmlonly
*/  


  