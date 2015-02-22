///////////////////////////////////////////////////////////////////////////////

NOTE: There are no underlying Standard C Library calls for certain methods 
      defined in the Cpl::Io::File::Api interface, e.g. isFile().  To statify
      all of the Cpl::Io::File::Api interface - you must compile and link the
      '_api/' directory.  This allows the application to use the C Library's
      FILE* stream calls for Input/Output files and then OS specific calls
      for the 'Api' interface.  

      NOTE: A 'null' implemenation is provided in the subdirectory '_nullapi/'
            for an application that does not use/need the methods in the 'Api'
            interface that do not have matching Standard C Library calls.


///////////////////////////////////////////////////////////////////////////////


  