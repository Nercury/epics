#ifndef H_LIBEPICSCLIENT_H
#  define H_LIBEPICSCLIENT_H

#  ifdef WIN32
#   ifdef DYNAMIC_LIB
#    ifdef epics_libclient_EXPORTS
#      define LIB_EPICS_CLIENT __declspec(dllexport)
#    else
#      define LIB_EPICS_CLIENT __declspec(dllimport)
#    endif
#   else
#    define LIB_EPICS_CLIENT
#   endif
#  else
#    define LIB_EPICS_CLIENT
#  endif

#endif
