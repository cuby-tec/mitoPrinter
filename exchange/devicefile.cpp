#include "devicefile.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <limits.h>
#include <sys/ioctl.h>

#include <string.h>

/**
  Used:
** Copyright (C)1999 Anca and Lucian Jurubita <ljurubita@hotmail.com>.
** All rights reserved.
** ** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details at www.gnu.org
** ** Rev. 1.0 - Feb. 2000
** Rev. 1.01 - March 2000
** Rev. 1.02 - June 2000
*/


//DeviceFile::DeviceFile()
//{

//}

int DeviceFile::init(const char *device)
{
    int opt_force = 0;
    int errorcode = 0;
    int ret = 0;
    long pid;
    int _fd;
    const char lockfilename[26] = "/var/lock/mito-cnccontrol";


    ret = snprintf(device_name,sizeof (device_name),"%s", device);

    assert(ret>0);

    lockfile = (char*)( malloc(512));
    assert (lockfile != 0);

    ret = snprintf(lockfile,512,lockfilename);
    assert(ret<512);

    _fd = open(lockfile, O_RDONLY);
    if (_fd >= 0 && !opt_force) {
        close(fd);
//        main_usage(3, "lockfile for port exists", device);
        free(lockfile);
        assert(_fd < 0 && opt_force);
    }


    if (_fd >= 0 && opt_force) {
        close(_fd);
        printf("lockfile for port exists, ignoring\n");
        unlock();
    }

    _fd = open(lockfile, O_RDWR | O_CREAT, 0444);
    if (_fd < 0 && opt_force) {
        printf("cannot create lockfile. ignoring\n");
//        lockfile = NULL;
        free(lockfile);
        goto force;
    }

//    if (_fd < 0)
//		main_usage(3, "cannot create lockfile for ", device);
    assert(_fd>=0);
    /* Kermit wants binary pid */
    if(_fd>0){
        pid = getpid();
        write(_fd, &pid, sizeof(long));
        close(_fd);
    }
    free(lockfile);
    assert(_fd>0);
    _isLocked = true;

force:
    /* open the device */
    _fd = open(device, O_RDWR | O_NONBLOCK);

    if (fd < 0) {
        unlock();
//		main_usage(2, "cannot open device", device);
    }
    assert(_fd>0);
    fd = _fd;

    printf("connected to %s\n", device);


    return errorcode;
}

void DeviceFile::unlock()
{
    if (lockfile)
        unlink(lockfile);
}

void DeviceFile::close_device()
{
    close(fd);
}

int DeviceFile::open_device(char *devname)
{
    int _fd;
    int errorcode = 0;
    if(fd<=0){
        _fd = open(devname, O_RDWR | O_NONBLOCK);
        if (_fd < 0) {
            unlock();
    //		main_usage(2, "cannot open device", device);
        }
        assert(_fd>0);
        fd = _fd;
        errorcode = 2;
        printf("connected to %s\n", device_name);
    }

    return errorcode;
}
