#!/bin/sh
#
#  vchanger-launch-umount.sh ( vchanger v.1.0.3 ) 2020-05-06
#
#  This script is used to run the vchanger-umount-uuid.sh script in
#  another [background] process launched by the at command in order
#  to prevent delays when invoked by a udev rule. 
#
VCHANGER_UMOUNT=/usr/libexec/vchanger/vchanger-umount-uuid.sh

# For some reason, nohup doesn't work, but "at now" does.  This may have to
# do with cgroups.
#nohup $VCHANGER_UMOUNT $1 </dev/null >/dev/null 2>&1 &
echo "$VCHANGER_UMOUNT $1 </dev/null >/dev/null 2>&1" | at now
