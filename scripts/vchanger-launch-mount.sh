#!/bin/sh
#
#  vchanger-launch-mount.sh ( vchanger v.1.0.3 ) 2020-05-06
#
#  This script is used to run the vchanger-mount-uuid.sh script as
#  a detached process and immediately exit. This is to prevent delays
#  when invoked by a udev rule. 
#
VCHANGER_MOUNT=/usr/libexec/vchanger/vchanger-mount-uuid.sh

# For some reason, nohup doesn't work, but "at now" does.  This may have to
# do with cgroups.
#nohup $VCHANGER_MOUNT $1 </dev/null >/dev/null 2>&1 &
echo "$VCHANGER_MOUNT $1 </dev/null >/dev/null 2>&1" | at now
