/** @file */
/** @namespace Cpl::Rte
@htmlonly
<pre>    

Overview
--------
The 'Rte' namespace is the root name space for a Data Model centric Run Time 
Environment (or RTE).  A RTE is the layer that provides a set of services and 
infra-structure that decouples the Application software/components from:
    o Other Application software/components
    o The physical target/platform it executes on
    o Specific communication protocols, drivers, operating systems, etc.  
    
The RTE also defines the mechanisms/framework/patterns for how Application 
shares data across significant software components and/or modules.  This concept 
of an RTE is based/derived from the AUTOSAR architecture (www.autosar.org). 

The Rte namespace is specific design choice for an RTE that is adapted from the
AUTOSAR model sans the Virtual Function Bus (VFB). The VFB allows Application
software components to interact with other Application software components
running on different physical targets.

The Rte namespace is multi-threaded data driven paradigm that provides a
Model-View-Controller (MVC) approach for Application components to interact with
each other. All application data that is shared across the Application
components is owned by the Model.  Individual Application components can read
(viewer) and/or write (controller) any of the data in the Model. Viewers can be
asynchronous, i.e. event driven, or synchronous, i.e. queries.  Controllers are
always synchronous. The data in the Model is always in RAM with the ability to
have a backing persistence storage to non-volatile  media.



Model-View-Controller
---------------------
The MVC paradigm is based around 'Points'. Points can be viewed as data items
and/or structures that are stored in the model and that Application uses viewers
or controllers to  read, write the data items.  Some terminology:

- A 'Model Base' is a collection of Model Points.  The Application is responsible
  for instantiating all of the Model Points in a Model Base.  Typically the
  life time scope of a Model Base is the life of the Application.

- A 'Model Point' is an instantiated Point within a Model Base.  A Model Point
  represents/is the canonical value/state of a Point.

- A 'Point' is a physical collection of data.  A Point can contain any 
  data structure(s).  Points are type safe.  Because each Point definition is
  unique this means that from the Application perspective there is not a base 
  class/interface that can be used to as a generic reference to Point. However,
  the great majority of the concrete Points will share common methods and 
  semantics.

- A 'Viewer Point' is an instantiated Point within a Application module that
  is used to read the contents of a Model Point.  All reads of an Model Point
  is done synchronously.  However, all Points support the ability for client(s)
  to register for asynchronous change notifications. There can be many Viewer 
  Points reading a single Model Point.  Typically a Viewer Point is created
  on the stack.  Note: There is no 'Viewer' data-type/class. The term Viewer
  Point refers to how a Point instance is used.

- A 'Controller Point' an instantiated Point within a Application module that 
  is used to synchronously write to a Model Point.  There can be many Controller 
  Points writing a single Model Point. Typically Controller Points are created 
  on the stack.  Note: There is no 'Controller' data-type/class. The term 
  Controller Point refers to how a Point instance is used.

</pre>    
@endhtmlonly
*/  


  