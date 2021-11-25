import platform
import subprocess
import sys
import os

#------------------------------------------------------------------------------
# Plaform independent interface
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
def make_link( link_name, source ):
    if ( platform.system() == 'Windows' ):
        return win32_make_link( link_name, source )
    else:
        return os.symlink( source, link_name )
    
#------------------------------------------------------------------------------
def remove_link( link_name ):
    if ( platform.system() == 'Windows' ):
        return win32_remove_link( link_name )
    else:
        return os.unlink( link_name )
    
#------------------------------------------------------------------------------
def is_link( link_name ):
    if ( platform.system() == 'Windows' ):
        return (win32_get_link_info( link_name, False ) != '')
    else:
        return os.path.islink(link_name)

#------------------------------------------------------------------------------
def get_link_source( link_name, raiseExceptionOnNotExist=True ):
    if ( platform.system() == 'Windows' ):
        return win32_get_link_info( link_name, raiseExceptionOnNotExist )
    else:
        return unix_get_link_info( link_name, raiseExceptionOnNotExist )




#------------------------------------------------------------------------------
# Windows specific implemenation (Requires Windows Vista or higher)
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
def win32_make_link( link_name, source ):
    # options
    if ( os.path.isfile(source) ):
        opt = ''
    elif ( os.path.isdir(source) ):
        opt = '/D '
    else:
        raise OSError("ERROR: Source for link does not exists: " + source )

    
    # Use windows MKLINK command to create the link
    cmd = 'mklink ' + opt + link_name + ' ' + source
    p   = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    p.communicate()
    if ( p.returncode ):
        raise OSError("ERROR: MKLINK command failed: " + cmd )
    
    
#------------------------------------------------------------------------------
def win32_remove_link( name ):
    
    # Do nothing if not an actual link
    if ( is_link( name ) ):
    
        # Unlink a FILE
        if ( os.path.isfile( name ) ):
            # Use windows DEL command to remove the link (becaus os.rmdir() errors out on 'access denied')
            cmd = 'del ' + name
            p   = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
            p.communicate()
            if ( p.returncode ):
                raise OSError("ERROR: The DEL command command failed: " + cmd )
            
        # Unlink a DIRECTORY
        else:
            # Use windows RMDIR command to remove the link (becaus os.remove() errors out on 'access denied')
            cmd = 'rmdir ' + name
            p   = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
            p.communicate()
            if ( p.returncode ):
                raise OSError( "ERROR: The RMDIR command failed: " + cmd )
        
    else:
        raise OSError( "ERROR: '{}' is not a symoblic link.".format(name) )
    
    
    
#------------------------------------------------------------------------------
def win32_get_link_info( name, raiseExceptionOnNotExist=True ):

    # Need parent directory for DIR command
    parent = os.path.dirname( name )
    if ( parent == '' ):
        parent = '.'
        
    # Use windows DIR command to get link info
    cmd = 'dir ' + parent + '| find "<SYMLINK" | find " ' + os.path.basename(name) + ' "'
    p   = subprocess.Popen( cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
    if ( p.returncode ):
        raise OSError( "ERROR: The DIR command failed: " + cmd )
      
    # Not found -->not a symbolick link
    r  = p.communicate()
    r0 = '' if r[0] == None else r[0].decode()
    r1 = '' if r[1] == None else r[1].decode()

    if ( r0 == '' ):
        if ( raiseExceptionOnNotExist ): 
            raise OSError( "ERROR: '{}' is not a symbolic link.".format(name) )
        return ''
        
    # extract link info and strip off leading/traling '[' ']'
    info = r0.split()
    return os.path.realpath( info[5][1:-1] )
    
    
    
def unix_get_link_info( name, raiseExceptionOnNotExist=True ):
    # Check if link is link
    if ( not os.path.islink( name ) ):
        if ( raiseExceptionOnNotExist ):
            raise OSError( "ERROR: '{}' is not a symbolic link.".format(name) )
        return ''

    return os.path.realpath( name )
