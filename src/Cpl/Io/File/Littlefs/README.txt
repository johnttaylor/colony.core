/** @namespace Cpl::Io::File::Littlefs

The 'Littlefs' namespace implements the CPL file abstractions using the open 
source 'littlefs' file system (https://github.com/littlefs-project/littlefs)

The 'littlefs' file system is currently support with the following caveats:

    o Short File names only, i.e. 8.3 with no spaces in the path/file names
    
    o The canonicalPath() method is not supported

    o The getCwd() method is not supported

    o Currently not file attributes are supported, e.g. no created, updated
      timestamps
    
    o ONLY the Cpl::Io::File standard directory separator - '/' - is supported,
      i.e. any Windows path separator and/or drive identifiers will NOT work

The implementation of the CPL file abstraction SUPPORTS multiple littlefs 
instances where each instance has a unique block driver instance.  Or said
another way, you have a single logical file system with different 'volumes
mounted' for each physical persistent storage media (e.g. multiple SPI data
flash ICs)

The default behavior is for a single littlefs instance and single physical
persistent media.  To use multiple littlefs instances the Api::initVolume()
method needs to be called multiple times - once for each littlefs instance.
In addition, each call to Api::initVolume() must supply a 'volumeName' which
becomes the logical root directory for the particular directory. ALL files and
directory names MUST be prefix with a 'volumeName' (i.e. the volumeName identifies
which littlefs to use.  For example:

    Cpl::Io::File::Littlefs::Api::initVolume( volume1, "A" );
    Cpl::Io::File::Littlefs::Api::initVolume( volume2, "B" );
    ...
    Cpl::Io::File::Output fd1( "/A/myfile", ... );  // Opened on volume1
    Cpl::Io::File::Output fd2( "/B/myfile", ... );  // Opened on volume1
    Cpl::Io::File::Api::createDirectory( "/A/mydirectory" );    // Created on volume1


*/  



  