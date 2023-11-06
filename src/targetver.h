
/* The following macros define the minimum required platform.  The minimum required platform
 * is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run
 * your application.  The macros work by enabling all features available on platform versions up to and
 * including the version specified.
 *
 * Modify the following defines if you have to target a platform prior to the ones specified below.
 * Refer to MSDN for the latest info on corresponding values for different platforms.
 */
#ifndef _TARGETVER_H_
#define _TARGETVER_H_ 1
#ifdef HAVE_WINDOWS_H
#ifndef WINVER
#define WINVER 0x0600
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#ifndef _WIN32_IE
#define _WIN32_IE 0x0700
#endif
#define _POSIX 1
#endif
#endif
