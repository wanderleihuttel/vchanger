/*  semaphore.c
 *
 *  This file is part of vchanger by Josh Fisher.
 *
 *  vchanger copyright (C) 2020 Josh Fisher
 *
 *  vchanger is free software.
 *  You may redistribute it and/or modify it under the terms of the
 *  GNU General Public License version 2, as published by the Free
 *  Software Foundation.
 *
 *  vchanger is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with vchanger.  See the file "COPYING".  If not,
 *  write to:  The Free Software Foundation, Inc.,
 *             59 Temple Place - Suite 330,
 *             Boston,  MA  02111-1307, USA.
 */

#include "config.h"

#ifndef HAVE_SEMAPHORE_H

#ifdef HAVE_WINDOWS_H
#include "targetver.h"
#include <windows.h>
#include <limits.h>
#include "win32_util.h"
#endif
/*
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
*/
#include "compat/semaphore.h"

#ifdef HAVE_WINDOWS_H

/*-------------------------------------------------
 *  Emulate POSIX.1-2001 sem_open() function using win32/win64 CreateSemaphoreW() function.
 *  On success address of semaphore, else NULL on error.
 *-------------------------------------------------*/
sem_t* sem_open(const char *name, int oflag, ...)
{
   DWORD rc;
   long mode, value;
   wchar_t *wname = NULL;
   size_t wname_sz = 0;
   sem_t *fd;
   va_list vl;

   va_start(vl, oflag);
   mode = (long)va_arg(vl, unsigned int);
   value = (long)va_arg(vl, unsigned int);
   va_end(vl);

   /* Convert path strings to UTF16 encoding */
   if (!AnsiToUTF16(name, &wname, &wname_sz)) {
      rc = ERROR_BAD_PATHNAME;
      errno = w32errno(rc);
      return NULL;
   }
   fd = (sem_t*)CreateSemaphoreW(NULL, value, LONG_MAX, wname);
   free(wname);
   if (fd == NULL) {
      errno = w32errno(GetLastError());
   }
   return fd;
}


/*-------------------------------------------------
 *  Emulate POSIX.1-2001 sem_close() function using win32/win64 CloseHandle() function.
 *  On success returns zero, else negative on error.
 *-------------------------------------------------*/
int sem_close(sem_t *sem)
{
   if (sem == NULL) {
      errno = EINVAL;
      return -1;
   }
   CloseHandle((HANDLE)sem);
   return 0;
}


/*-------------------------------------------------
 *  Emulate POSIX.1-2001 sem_unnlink() function by doing nothing. Windows will
 *  automatically unlink the object when the last open handle is closed.
 *  Returns zero.
 *-------------------------------------------------*/
int sem_unlink(const char *name)
{
   return 0;
}


/*-------------------------------------------------
 *  Emulate POSIX.1-2001 sem_post() function using win32/win64 ReleaseSemaphore() function.
 *  On success returns zero, else negative on error.
 *-------------------------------------------------*/
int sem_post(sem_t *sem)
{
   if (sem == NULL) {
      errno = EINVAL;
      return -1;
   }
   ReleaseSemaphore((HANDLE)sem, 1, NULL);
   return 0;
}


/*-------------------------------------------------
 *  Emulate POSIX.1-2001 sem_wait() function using win32/win64 WaitForSingleObjectEx() function.
 *  On success returns zero, else negative on error.
 *-------------------------------------------------*/
int sem_wait(sem_t *sem)
{
   DWORD reason;
   if (sem == NULL) {
      errno = EINVAL;
      return -1;
   }
   reason = WaitForSingleObjectEx((HANDLE)sem, INFINITE, FALSE);
   if (reason == 0) return 0;
   switch (reason) {
   case WAIT_IO_COMPLETION:
      errno = EINTR;
      break;
   case WAIT_TIMEOUT:
      errno = ETIMEDOUT;
      break;
   default:
      errno = EINVAL;
      break;
   }
   return -1;
}


/*-------------------------------------------------
 *  Emulate POSIX.1-2001 sem_timedwait() function using win32/win64 WaitForSingleObjectEx()
 *  function.
 *  On success returns zero, else negative on error.
 *-------------------------------------------------*/
int sem_timedwait(sem_t *sem, const struct timespec *timeout)
{
   DWORD reason, mt;
   if (sem == NULL) {
      errno = EINVAL;
      return -1;
   }
   /* semaphore.h functions use absolute time and Windows needs a time interval */
   timeout->tv_sec -= time(NULL);
   mt = timeout->tv_sec * 1000 + (timeout->tv_nsec / 1000000);
   reason = WaitForSingleObjectEx((HANDLE)sem, mt, FALSE);
   if (reason == WAIT_OBJECT_0) return 0;
   switch (reason) {
   case WAIT_IO_COMPLETION:
      errno = EINTR;
      break;
   case WAIT_TIMEOUT:
      errno = ETIMEDOUT;
      break;
   default:
      errno = EINVAL;
      break;
   }
   return -1;
}


/*-------------------------------------------------
 *  Emulate POSIX.1-2001 sem_trywait() function using win32/win64 WaitForSingleObjectEx()
 *  function.
 *  On success returns zero, else negative on error.
 *-------------------------------------------------*/
int sem_trywait(sem_t *sem)
{
   DWORD reason;
   if (sem == NULL) {
      errno = EINVAL;
      return -1;
   }
   reason = WaitForSingleObjectEx((HANDLE)sem, 0, FALSE);
   if (reason == 0) return 0;
   errno = EAGAIN;
   return -1;
}

#endif

#endif
