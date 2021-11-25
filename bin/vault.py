"""Collection of helper functions"""

from ftplib import FTP
import os, configparser, tempfile
import utils
import six 

# globals
_found     = 0
_lastfound = ''
_count     = 0
_n         = 0
_shortflag = False    

#------------------------------------------------------------------------------
def display_supported_types():
    print("Supported Vault Types:")
    print("----------------------")
    print("ftp-anon    ; Anonymous FTP")
    print("ftp-user    ; User account FTP")


#------------------------------------------------------------------------------
def create_template_vault_config( vault_type ):
    # Create content
    cfg = configparser.RawConfigParser(allow_no_value=True)
    cfg.optionxform = str
    cfg.add_section( 'primary' )
    cfg.add_section( 'mirror1' )
    
    # Select vault type:
    if ( vault_type == 'ftp-anon' ):
        _ftp_anon( cfg, 'primary' )
    elif ( vault_type == 'ftp-user' ):
        _ftp_user( cfg, 'primary' )
    else:
        exit ("ERROR. Unsupported vault type: " + vault_type )
        
    return cfg


#------------------------------------------------------------------------------
def push_archive( vaultfile, archive, overwrite=False ):
 
    # Read vault file
    cfg, vtype = _read_primary( vaultfile )

    # Select vault type
    if ( vtype == 'ftp-anon' or vtype == 'ftp-user' ):
        _push_ftp( vaultfile, cfg, archive, overwrite )
        
    else:
        exit( "ERROR: Unsupported vault type: {} in vault file".format(vault_type, vaultfile) )
       

def list_vault( vaultfile, filter, shortflag=False ):
 
    # Read vault file
    cfg, vtype = _read_primary( vaultfile )

    # Select vault type
    if ( vtype == 'ftp-anon' or vtype == 'ftp-user' ):
        _list_ftp( vaultfile, cfg, filter, shortflag )
        
    else:
        exit( "ERROR: Unsupported vault type: {} in vault file".format(vault_type, vaultfile) )
       
       
def pull_archive( vaultfile, archive ):
 
    # Read vault file
    cfg, vtype = _read_primary( vaultfile )
    
    # Select vault type
    if ( vtype == 'ftp-anon' or vtype == 'ftp-user' ):
        return _pull_ftp( vaultfile, cfg, archive )
        
    else:
        exit( "ERROR: Unsupported vault type: {} in vault file".format(vault_type, vaultfile) )
       

def remove_archive( vaultfile, archive ):

    # Read vault file
    cfg, vtype = _read_primary( vaultfile )
    
    # Select vault type
    if ( vtype == 'ftp-anon' or vtype == 'ftp-user' ):
        return _remove_ftp( vaultfile, cfg, archive )
        
    else:
        exit( "ERROR: Unsupported vault type: {} in vault file".format(vault_type, vaultfile) )
       

def _read_primary( vaultfile ):
    
    # Read vault file
    cfg = six.moves.configparser.RawConfigParser(allow_no_value=True)
    cfg.optionxform = str
    if ( not os.path.isfile(vaultfile) ):
        exit( "ERROR: Vault file - {} - does not exist".format( vaultfile ) )
    else:
        cfg.read( vaultfile )

    # Parse 'type' from the primary vault
    if ( not cfg.has_section( 'primary' ) ):
        exit( "ERROR: Missing [primary] section in vault file ({}).".format( vaultfile ) )
    if ( not cfg.has_option( 'primary', 'type' ) ):
        exit( "ERROR: Missing [primary.type] field in vault file ({}).".format( vaultfile ) )
    vtype = cfg.get( 'primary', 'type' )
         

    return (cfg, vtype)
       


#------------------------------------------------------------------------------
# FTP - User Account
#------------------------------------------------------------------------------
def _ftp_user( cfg, section ):
    _ftp_anon( cfg, section )
    cfg.set( section, 'user',      '<user_account>' )
    cfg.set( section, 'password',  '<user_passowrd>' )


#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# FTP - Anonymous
#------------------------------------------------------------------------------
def _ftp_anon( cfg, section ):
    cfg.set( section, 'type',      'ftp-anon' )
    cfg.set( section, 'host',      '<address_of_ftp_site>' )
    cfg.set( section, 'vault_dir', '<dir_where_the_vault_is_located>' )


#------------------------------------------------------------------------------
def _ftp_get_host_and_dir( cfg, vaultfile ):

    # get ftp url
    if ( not cfg.has_option( 'primary', 'host' ) ):
        exit( "ERROR: Missing [primary.host] field in vault file ({}).".format( vaultfile ) )
    host = cfg.get( 'primary', 'host' )
    
    # get vault directory
    if ( not cfg.has_option( 'primary', 'vault_dir' ) ):
        exit( "ERROR: Missing [primary.vault_dir] field in vault file ({}).".format( vaultfile ) )
    dir = cfg.get( 'primary', 'vault_dir' )
    
    return (host, dir)


def _ftp_connect_and_cd( host, dir, cfg ):

    # Connect to the FTP server
    try:
        utils.print_verbose( "Connecting to {}...".format( host ) )
        ftp = FTP( host )
    except Exception as ex:
        exit( "ERROR: Unable to connect to: " + host )
    
    # Login
    try:
        utils.print_verbose( "Logging in..." )
        if ( cfg.get('primary', 'type' ) == 'ftp-anon' ):
            utils.print_verbose( "ftp-anon..." );
            ftp.login()
        else:
            utils.print_verbose( "ftp-user.  user={}, passwd={} ...".format(cfg.get('primary', 'user'),cfg.get('primary', 'password')) );
            ftp.login(cfg.get('primary', 'user'),cfg.get('primary', 'password'))
            
    except Exception as ex:
        if ( cfg.get('primary', 'type' ) == 'ftp-anon' ):
            exit( "ERROR: Unable to login in anonymously into: " + host )
        else:
            exit( "ERROR: Unable to login in as user: {} into: {}".format( cfg.get('primary', 'user'), host) )
            
    
    # Get welcome message
    try:
        utils.print_verbose( ftp.getwelcome() )
    except:
        pass
 
    # Goto vault dir
    if ( dir != "" ):
        try:
            ftp.cwd(dir)
        except Exception as ex:
            exit( "ERROR: Vault directory - {} - does not exist on FTP site.".format( dir ) )
        
    return ftp

#------------------------------------------------------------------------------
def _push_ftp( vaultfile, cfg, archive, overwrite=False ):

    # get host and vault dir
    host, dir = _ftp_get_host_and_dir(cfg, vaultfile)
    
               
    # Connect to the FTP server
    f   = os.path.basename( archive )
    ftp = _ftp_connect_and_cd( host, dir, cfg )
        
    # check if archive already exists
    try:
        _reset_found()

        # Some FTP servers will generate an 'cannot find file' error here -->which 
        # generates an exception -->which is okay to ignore since it means the file 
        # is not present.
        try:
            ftp.retrlines('LIST ' + f, _cbfound ) 
        except Exception as ex:
            pass 
                    
        if ( _found_count() > 0 ):
            if ( overwrite ):
                utils.print_warning( "Overwriting file " + f )
            else:
                exit( "ERROR: File - {} - already exists on the FTP site".format( f ) )
        
        blocksize = 1024 * 50
        asize     = os.path.getsize(archive)        
        _set_progress_block( asize, blocksize )
        utils.print_verbose( "Transferring file {} ".format( f ), True )
        ftp.storbinary( "STOR " + f, open(archive,"rb"), blocksize, _cbprogress )
        utils.print_verbose(" Complete",False,True)
        
    except Exception as ex:
        print(ex)
        exit ( "ERROR: Unable to transfer file {}.".format( archive ) )
            
    ftp.close()
    
    
def _cbprogress( a ):
    global _n
    global _count

    _count += 1
    if ( _count > _n ):
        _count = 0
        utils.print_verbose( ".", True, True )

def _set_progress_block( fsize, bsize ):
    global _n
    global _count
    _count = 0
    _n     = int( ((fsize / bsize) / 50) + 0.5)
    
def _cbfound(s):
    global _found
    global _lastfound
    _found += 1
    _lastfound = s
    
def _reset_found():
    global _found
    _found = 0
    
    
def _found_count():
    global _found
    return _found
    
    
#------------------------------------------------------------------------------
def _list_ftp( vaultfile, cfg, filter, shortflag=False ):

    # get host and vault dir
    host, dir = _ftp_get_host_and_dir(cfg, vaultfile)
    
    # Connect to the FTP server
    ftp = _ftp_connect_and_cd( host, dir, cfg )
        
    # check if archive already exists
    try:
        _set_short_flag( shortflag )
        ftp.retrlines( 'LIST ' + filter, _cblist )
        
    except Exception as ex:
        print(ex)
        exit ( "ERROR: Unable to list vault directory." )
            
    ftp.close()


def _set_short_flag( shortflag ):
    global _shortflag
    _shortflag = shortflag    
    

def _cblist( line ):
    global _shortflag
    
    t = line.split()
    if ( _shortflag ):
        print(t[len(t)-1])
    else:
        print(line)


#------------------------------------------------------------------------------
def _pull_ftp( vaultfile, cfg, archive ):

    # get host and vault dir
    host, dir = _ftp_get_host_and_dir(cfg, vaultfile)
    
    # Connect to the FTP server
    ftp =_ftp_connect_and_cd( host, dir, cfg )
    f   = os.path.basename( archive )


    # Does the arhive exist (and only ONCE)
    try:
        _reset_found()
        ftp.retrlines('LIST ' + f + '*', _cbfound ) 
        if ( _found_count() < 1 ):
            exit( "ERROR: Archive - {} - does NOT exist on the FTP site".format( f ) )
        elif ( _found_count() > 1 ):
            exit( "ERROR: Multiple Archive files (of name: {}) exist -- unable to resolve which archive to pull".format(f) )

        # get the actual archive name
        global _lastfound
        t     = _lastfound.split()
        f     = t[len(t)-1]
        asize = int(t[len(t)-2])
        
    except Exception as ex:
        print(ex)
        exit ( "ERROR: Unable to find archive file {}.".format( f ) )


    # Create a temporary file to hold the pulled archive
    global _fd
    _fd = tempfile.TemporaryFile()
        
    # Transfer the file
    try:
            
        # Setup 'transfer in progress' magic numbers
        blocksize = 1024 * 50
        _set_progress_block( asize, blocksize )

        # Get the actual archive name
        utils.print_verbose( "Transferring file {} ".format( f ), True )
        ftp.retrbinary( "RETR " + f, _cbretrieve, blocksize )
        utils.print_verbose(" Complete",False,True)
        ftp.close()
        
    except Exception as ex:
        _fd.close()
        print(ex)
        exit ( "ERROR: Unable to transfer file {} (err={}).".format( f, ex ) )
                
 
    # Return the file handle of the download archive file
    _fd.seek(0)
    return _fd
       
    
def _cbretrieve( data ):
    # write to the file
    global _fd
    _fd.write(data)
    
    # Manage progress bar
    global _n
    global _count
    _count += 1
    if ( _count > _n ):
        _count = 0
        utils.print_verbose( ".", True, True )

    
def _set_progress_block( fsize, bsize ):
    global _n
    global _count
    _count = 0
    _n     = int( ((fsize / bsize) / 50) + 0.5)
    

#------------------------------------------------------------------------------
def _remove_ftp( vaultfile, cfg, archive ):

    # get host and vault dir
    host, dir = _ftp_get_host_and_dir(cfg, vaultfile)
    
               
    # Connect to the FTP server
    f   = os.path.basename( archive )
    ftp = _ftp_connect_and_cd( host, dir, cfg )
        
    # check if archive already exists
    try:
        _reset_found()
        ftp.retrlines('LIST ' + f, _cbfound ) 
        if ( _found_count() == 0 ):
            exit( "ERROR: Archive file - {} - does NOT exist on the FTP site".format( f ) )
        ftp.delete(f)
        
    except Exception as ex:
        print(ex)
        exit ( "ERROR: Unable to delete file {}.".format( archive ) )
            
    ftp.close()
