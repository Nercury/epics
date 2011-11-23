#ifndef H_LIBKEYNET_H
#  define H_LIBKEYNET_H

#  ifdef WIN32
#   ifdef DYNAMIC_LIB
#    ifdef key_net_EXPORTS
#      define LIB_KEY_NET __declspec(dllexport)
#    else
#      define LIB_KEY_NET __declspec(dllimport)
#    endif
#   else
#    define LIB_KEY_NET
#   endif
#  else
#    define LIB_KEY_NET
#  endif

#endif
