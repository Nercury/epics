#ifndef H_LIBKEYCOMMON_H
#  define H_LIBKEYCOMMON_H

#  ifdef WIN32
#   ifdef DYNAMIC_LIB
#    ifdef key_common_EXPORTS
#      define LIB_KEY_COMMON __declspec(dllexport)
#    else
#      define LIB_KEY_COMMON __declspec(dllimport)
#    endif
#   else
#    define LIB_KEY_COMMON
#   endif
#  else
#    define LIB_KEY_COMMON
#  endif

#endif
