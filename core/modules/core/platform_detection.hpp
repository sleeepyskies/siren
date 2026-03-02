#pragma once

#if defined(unix) || defined(__unix) || defined(__unix__)
#define SIREN_UNIX
#endif
#if defined(__APPLE__)
#define SIREN_APPLE
#endif
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#define SIREN_WINDOWS
#endif
