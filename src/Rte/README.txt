/** @file */
/** @namespace Rte
@htmlonly
<pre>    

Overview
--------
The 'Rte' namespace is the root name space for a Data Model centric Run Time 
Environment (or RTE).  A RTE is the layer that provides a set of services and 
infra-structure that decouples the Application software/components from the
physical target/platform it executes on as well as decoupling it from specific
communication protocols, drivers, operating systems, etc.  The RTE also defines
the mechanisms/framework/patterns for how Application shares data across
signficant software components and/or modules.  This concept of an RTE is
based/derived from the AUTOSAR archicture (www.autosar.org). 

The Rte namespace is specific design choice for an RTE that is adapted from the
AUTOSAR model sans the Virtual Function Bus (VFB) (The VFB allows Application
software components to interact with other Application software components
running on different physical targets).

The Rte namespace is multi-threaded data driven paradigm that provides a
Model-View-Controller (MVC) approach for Application components to interact with
each other. All application data that is shared across the Applicaiton
components is owned by the Model.  Individual Application components can read
(viewer) and/or write (controller) any of the data in the Model. Viewers can be
asynchronous, i.e. event driven, or synchronous, i.e. queries.  Controllers are
always synchronous. The data in the Model is always in RAM with the ability to
have a backing persisetance storage to non-volitile  media.



Model-View-Controller
---------------------
The MVC paradigm is based around 'Points'. Points can be viewed as data items
and/or structures that are stored in the model and that Application uses viewers
or controllers to  read, write the data items.  Some terminology:

- A 'Point' is a physical collection of data.  The Point mechanisms provide the
  ability to create fairly sophiscated data structures. A Point consist of one 
  or more 'Tuples' (think a data structure contain structures and/or an array of
  structures).  A Tuple (think data structure) contains one or more 'Elements'.
  A Element is the primitive data unit, e.g. a boolean, a integer, a string,
  etc.

- A 'Model Point' provides access to a Point that is owned by the model. The 
  life time scope of a Model Point is the life time scope of the Point (in the
  Application's model).

- A 'Viewer Point' is what the Application uses for asynchronous (i.e. change
  based notifications) reads of a Model Point.  There can be many Viewer Points
  reading a single Model Point.  The life time scope of a Viewer Point is the
  life time of Application component that it is contained in.

- A 'Query Point' is a 'viewer' that is used to read a Model Point 
  synchronously. There can be many Query Points reading a single Model Point. 
  Typically Query Points are created on the stack.
  
- A 'Controller Point' is what the Application uses to sychronously write to
  a Model Point.  There can be many Controller Points writing a single Model 
  Point. Typically Controller Points are created on the stack. 

- A 'Container Point' is Point that has N Tuples where all of the Tuples are
  the same type, i.e. contain the same set of Elements.  A Container Point
  can by used/viewed by the Application as an array of Tuples, or a list of 
  Tuples.

</pre>    
@endhtmlonly
*/  


  