/* $Id: ioctl.c,v 1.1.1.2 2005/12/07 10:50:34 sonic_amiga Exp $
 *
 *      ioctl.c - set file control information
 *
 *      Copyright � 1994 AmiTCP/IP Group, 
 *                       Network Solutions Development Inc.
 *                       All rights reserved.
 */

#include <sys/param.h>
#include <sys/socket.h>

int ioctl(int fd, unsigned int request, char *argp)
{
  int success;

  /*
   * IoctlSocket will return EBADF if the d is not socket
   */
  success = IoctlSocket(fd, request, argp);

  /*
   * Maybe the EBADF should be converted to EINVAL if the fd is an usual file?
   */

  return success;
}

