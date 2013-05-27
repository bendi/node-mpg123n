#define strcasecmp _strcmpi
#define strncasecmp _strnicmp
#define strdup _strdup
#define snprintf _snprintf

#define STDIN_FILENO stdin
#define STDOUT_FILENO stdout

#define HAVE_STRERROR 1
#define HAVE_SYS_TYPES_H 1

/* We want some frame index, eh? */
#define FRAME_INDEX 1
#define INDEX_SIZE 1000

/* also gapless playback! */
#define GAPLESS 1
/* #ifdef GAPLESS
#undef GAPLESS
#endif */

/* #define DEBUG
#define EXTRA_DEBUG */

/* defined in "gyp"
#define REAL_IS_FLOAT */

#define inline __inline

/* we are on win32 */
#define HAVE_WINDOWS_H

/* use the unicode support within libmpg123 */
#ifdef UNICODE
	#define WANT_WIN32_UNICODE
#endif

#define DEFAULT_OUTPUT_MODULE "win32"

/* Name of package */
#define PACKAGE "mpg123"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "mpg123-devel@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "mpg123"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "mpg123 1.15.3"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "mpg123"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.15.3"

/* The suffix for module files. */
#define MODULE_FILE_SUFFIX ".lib"

#define PKGLIBDIR "build/Release"

#define HAVE_WS2TCPIP_H

#define HAVE_LIMITS_H

#ifndef NOXFERMEM
#define NOXFERMEM
#endif

#if !defined(__MINGW32__) && (!defined(_MSC_VER) || _MSC_VER<1600)
#include <BaseTsd.h>
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef SIZE_T size_t;
typedef SSIZE_T ssize_t;
#else
#endif
