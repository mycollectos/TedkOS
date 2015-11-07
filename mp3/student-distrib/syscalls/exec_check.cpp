#include "exec.h"
#include <inc/fs/kiss.h>
#include <inc/fs/kiss_wrapper.h>

using namespace syscall_exec;

static const int NORMAL_FILE = 2;
/*
 *  MagicNum: 0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46.
 */

// Check whether exists, and whether executable.
// Return 0 if not exist or not an executable.
int8_t is_kiss_executable(const uint8_t* file)//filename
{
    //check dentry
    dentry_t dentry;
    if( read_dentry_by_name(file,&dentry) == -1 )//cannot find
        return 0;
    if(dentry.filetype != NORMAL_FILE)//not regular file
        return 0;
    uint8_t buf[4] = {};
    uint32_t len = read_data(dentry.inode,0,buf,sizeof(buf));
    if( len < 4)//read error, or input file length error
        return 0;
    if(buf[0] == 0x7f &&
        buf[1] == 0x45 &&
        buf[2] == 0x4c &&
        buf[3] == 0x46
    )
        return 1;
    else
        return 0;
}