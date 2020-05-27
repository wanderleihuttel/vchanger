/* mymutex.cpp
 *
 *  Copyright (C) 2017-2020 Josh Fisher
 *
 *  This program is free software. You may redistribute it and/or modify
 *  it under the terms of the GNU General Public License, as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  See the file "COPYING".  If not,
 *  see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#ifdef HAVE_WINDOWS_H
#include "targetver.h"
#include <windows.h>
#include "win32_util.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SEMAPHORE_H
#include <semaphore.h>
#endif

#include "compat/semaphore.h"
#include "loghandler.h"
#include "mypopen.h"


/*
 *  Function to create a mutex owned by the caller. Waits up to max_wait seconds
 *  for the mutex to be created. If max_wait is negative, waits indefinitely. If
 *  max_wait is zero, tries once to create mutex and does not block.
 *  On success, returns the handle of a named mutex. On error, returns zero and
 *  sets errno appropriately.
 */
void* mymutex_create(const char *storage_name)
{
   char lockname[4096];

   if (!storage_name || !storage_name[0]) {
      /* Only create named mutex */
      errno = EINVAL;
      return 0;
   }
#ifdef HAVE_WINDOWS_H
   snprintf(lockname, sizeof(lockname), "vchanger-%s", storage_name);
#else
   snprintf(lockname, sizeof(lockname), "/vchanger-%s", storage_name);
#endif
   return (void*)sem_open(lockname, O_CREAT, 0770, 1);
}


/*
 *  Function to lock an opened mutex given by fd.
 *  On success, returns zero. On error, returns -1 and
 *  sets errno appropriately.
 */
int mymutex_lock(void *fd, time_t wait_sec)
{
   struct timespec ts;
   if (wait_sec == 0) return sem_trywait((sem_t*)fd);
   ts.tv_sec = time(NULL) + wait_sec;  /* semaphore.h functions use absolute time */
   ts.tv_nsec = 0;
   return sem_timedwait((sem_t*)fd, &ts);
}


/*
 *  Function to unlock an opened mutex given by fd.
 *  On success, returns zero. On error, returns -1 and
 *  sets errno appropriately.
 */
int mymutex_unlock(void *fd)
{
   return sem_post((sem_t*)fd);
}


/*
 *  Function to destroy a mutex owned by the caller.
 *  On success, returns zero. On error, returns -1 and
 *  sets errno appropriately.
 */
void mymutex_destroy(const char *name, void *fd)
{
   if (fd) {
      sem_post((sem_t*)fd);
      sem_close((sem_t*)fd);
   }
   if (name && name[0]) sem_unlink(name);
}

