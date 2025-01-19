# Outcast
The contents of this directory are external repositories that have been 
'copied' into the colony.core repo using the [Outcast tool](https://github.com/johnttaylor/Outcast).  The external repos are copied 'as is' and no additional modifications are needed - except for those listed below

## Manual Edits
### catch2
The Catch2 +v3.x unit test framework is used automated unit testing by the
colony.core repository.  The following file needs to be created and store in the `xsrc/catch2` directory tree

File: `xsrc/catch2/src/catch2/catch_user_config.hpp`

Contents:
```
#ifndef CATCH_USER_CONFIG_HPP_INCLUDED
#define CATCH_USER_CONFIG_HPP_INCLUDED

// Stripped down catch configuration file.

#define CATCH_CONFIG_DEFAULT_REPORTER "console"
#define CATCH_CONFIG_CONSOLE_WIDTH    80

#endif  // CATCH_USER_CONFIG_HPP_INCLUDED
```

### littlefs
Littlefs supports running on a Host (e.g. Linux, Windows) and using the host's underlying file system for its block driver. To enable building littlefs using the Microsoft compiler without warnings, the following edits need to be made

File: `xsrc/littlefs/lfs.c` - Add the following #ifdef at the top of the file
      
```
// EDIT: JTT - Added this to suppress warning when using the Visual Studio compiler
#ifdef _MSC_VER
#pragma warning( disable : 4244 4804 )
#endif
```

File: `xsrc/littlefs/lfs_utils.h` - Add the following #ifdef at the top of the file
```
// EDIT: JTT - Added this to suppress warning when using the Visual Studio compiler
#ifdef _MSC_VER
#pragma warning( disable : 4146 )
#endif
```            