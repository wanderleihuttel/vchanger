/* bconsole.cpp
 *
 *  Copyright (C) 2008-2018 Josh Fisher
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#else
#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#include "compat_defs.h"
#include "util.h"
#include "loghandler.h"
#include "mymutex.h"
#include "mypopen.h"
#include "vconf.h"
#include "bconsole.h"

#ifndef HAVE_WINDOWS_H

/*
 *  Function to issue command in Bacula console.
 *  Returns zero on success, or errno if there was an error running the command
 *  or a timeout occurred.
 */
static int issue_bconsole_command(const char *bcmd)
{
   int pid, rc, n, len, fno_in = -1, fno_out = -1;
   struct timeval tv;
   fd_set rfd;
   tString cmd, tmp;
   char buf[4096];

   /* Build command line */
   cmd = conf.bconsole;
   if (cmd.empty()) return 0;
   if (!conf.bconsole_config.empty()) {
      cmd += " -c ";
      cmd += conf.bconsole_config;
   }
   cmd += " -n -u 30";
   /* Start bconsole process */
   vlog.Debug("running '%s'", cmd.c_str());
   pid = mypopen_raw(cmd.c_str(), &fno_in, &fno_out, NULL);
   if (pid < 0) {
      rc = errno;
      vlog.Error("bconsole run failed errno=%d", rc);
      errno = rc;
      return rc;
   }
   /* Wait for bconsole to accept input */
   tv.tv_sec = 30;
   tv.tv_usec = 0;
   FD_ZERO(&rfd);
   FD_SET(fno_in, &rfd);
   rc = select(fno_in + 1, NULL, &rfd, NULL, &tv);
   if (rc == 0) {
      vlog.Error("timeout waiting to send command to bconsole");
      close(fno_in);
      close(fno_out);
      errno = ETIMEDOUT;
      return ETIMEDOUT;
   }
   if (rc < 0) {
      rc = errno;
      vlog.Error("errno=%d waiting to send command to bconsole", rc);
      close(fno_in);
      close(fno_out);
      errno = rc;
      return rc;
   }
   /* Send command to bconsole's stdin */
   vlog.Debug("sending bconsole command '%s'", bcmd);
   len = strlen(bcmd);
   n = 0;
   while (n < len) {
      rc = write(fno_in, bcmd + n, len - n);
      if (rc < 0) {
         rc = errno;
         vlog.Error("send to bconsole's stdin failed errno=%d", rc);
         close(fno_in);
         close(fno_out);
         errno = rc;
         return rc;
      }
      n += rc;
   }
   if (write(fno_in, "\n", 1) != 1) {
      rc = errno;
      vlog.Error("send to bconsole's stdin failed errno=%d", rc);
      close(fno_in);
      close(fno_out);
      errno = rc;
      return rc;
   }

   /* Wait for bconsole process to finish */
   close(fno_in);
   pid = waitpid(pid, &rc, 0);
   if (!WIFEXITED(rc)) {
      vlog.Error("abnormal exit of bconsole process");
      close(fno_out);
      return EPIPE;
   }
   if (WEXITSTATUS(rc)) {
      vlog.Error("bconsole: exited with rc=%d", WEXITSTATUS(rc));
      close(fno_out);
      return WEXITSTATUS(rc);
   }

   /* Read stdout from bconsole */
   vlog.Debug("bconsole: bconsole terminated normally");
   memset(buf, 0, sizeof(buf));
   tmp.clear();
   rc = read(fno_out, buf, 4095);
   while (rc > 0) {
      buf[rc] = 0;
      tmp += buf;
      rc = read(fno_out, buf, 4095);
   }
   if (rc < 0) {
      rc = errno;
      vlog.Error("errno=%d reading bconsole stdout", rc);
      close(fno_out);
      errno = rc;
      return rc;
   }
   close(fno_out);
   vlog.Debug("bconsole output:\n%s", tmp.c_str());

   return 0;
}


/*
 *  Function to fork a new process and issue commands in Bacula console to
 *  perform update slots and/or label new volumes using barcodes.
 */
void IssueBconsoleCommands(bool update_slots, bool label_barcodes)
{
   tString cmd;

   /* Check if update needed */
   if (!update_slots && !label_barcodes) return; /* Nothing to do */

   /* Perform update slots command in bconsole */
   if (update_slots) {
      tFormat(cmd, "update slots storage=\"%s\" drive=\"0\"", conf.storage_name.c_str());
      if(issue_bconsole_command(cmd.c_str())) {
         vlog.Error("WARNING! 'update slots' needed in bconsole");
      }
      vlog.Info("bconsole update slots command success");
   }

   /* Perform label barcodes command in bconsole */
   if (label_barcodes) {
      tFormat(cmd, "label storage=\"%s\" pool=\"%s\" barcodes\nyes\nyes\n", conf.storage_name.c_str(),
            conf.def_pool.c_str());
      if (issue_bconsole_command(cmd.c_str())) {
         vlog.Error("WARNING! 'label barcodes' needed in bconsole");
      }
      vlog.Info("bconsole label barcodes command success");
   }
}

#else
/*
 *  Bconsole interaction is not currently supported on Windows
 */
void IssueBconsoleCommands(bool update_slots, bool label_barcodes)
{
   return;
}
#endif
