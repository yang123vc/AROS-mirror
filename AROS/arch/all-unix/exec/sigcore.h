#ifndef _SIGCORE_H
#define _SIGCORE_H

#ifdef __linux__
#   define __KERNEL__
#endif
#include <signal.h>
#ifdef __KERNEL__
#   undef __KERNEL__
#endif

#ifdef __linux__
typedef struct sigcontext_struct sigcontext_t;
#   define SIGHANDLER	   linux_sighandler
#   define SIGHANDLER_T    SignalHandler

#   define SP(sc)      (sc->esp)
#   define FP(sc)      (sc->ebp)
#   define PC(sc)      (sc->eip)
#endif /* __linux__ */

#endif /* _SIGCORE_H */
